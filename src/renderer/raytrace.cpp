#include "common.h"

#include <boost/thread.hpp>
#include "math/math.h"
#include "renderer/accel/kdtree.h"
#include "renderer/intersection.h"
#include "renderer/raytrace.h"
#include "renderer/scene.h"
#include "renderer/entity/entity.h"
#include "renderer/camera/camera.h"
#include "renderer/light.h"
#include "util/random.h"

#define USE_KDTREE

namespace raytrace {;

namespace {

Color ColorFromRay(Scene const& scene, KdTree const& kdtree,
                   Ray const& ray, int depth, Intersection *out = NULL,
                   bool *hit_out = NULL) {
  if (depth == 0) return color::black;

  Intersection intersection;
#ifdef USE_KDTREE
  bool hit = kdtree.Intersect(ray, &intersection);
#else
  bool hit = scene.Intersect(ray, &intersection);
#endif
  if (hit_out) *hit_out = hit;
  Color color = color::black;

  if (!hit) {
    return scene.background_color;
  }

  if (out) {
    *out = intersection;
  }

  Point ipoint = ray(intersection.distance);
  Entity const& entity = *intersection.entity;
  Texture *texture = entity.texture.get();
  Color tex_color = texture->pigment->color_at(intersection.geometry);
  color = tex_color * texture->finish->ambient;

  for (vector<boost::shared_ptr<Light> >::const_iterator it = scene.Lights().begin();
      it != scene.Lights().end(); ++it) {
    Light const& light = **it;
    double light_multiplier = 1.0 / (light.rows * light.cols);

    for (int i = 0; i < light.rows; ++i) {
      for (int j = 0; j < light.cols; ++j) {
      Ray shadow_ray;
      Color c = light.Sample(ipoint + intersection.geometry.normal * EPS, &shadow_ray, i, j);

#ifdef USE_KDTREE
      if (!kdtree.Intersect(shadow_ray, NULL)) {
#else
      if (!scene.Intersect(shadow_ray, NULL)) {
#endif
        // Diffuse
        if (texture->finish->diffuse > 0.0) {
          double d = Dot(intersection.geometry.normal, shadow_ray.direction);
          if (d > 0.0) {
            d *= texture->finish->diffuse;
            color += (d * tex_color * c) * light_multiplier;
          }
        }

        // Specular
        if (texture->finish->specular > 0.0) {
            Vector refl_dir =
              shadow_ray.direction - 2 * Dot(shadow_ray.direction, intersection.geometry.normal)
                                       * intersection.geometry.normal;
            double d = Dot(ray.direction, refl_dir);
            if (d > 0.0) {
              color += (pow(d, texture->finish->exponent)
                * texture->finish->specular * tex_color) * light_multiplier;
            }
          }
        }
      }
    }
  }

  // Calculate reflection
  if (texture->finish->reflection > 0.0) {
    Vector refl_dir =
      ray.direction - 2 * Dot(ray.direction, intersection.geometry.normal)
                        * intersection.geometry.normal;
    Ray refl_ray = Ray(ipoint + intersection.geometry.normal * EPS, refl_dir);
    color += texture->finish->reflection
              * tex_color
              * ColorFromRay(scene, kdtree, refl_ray, depth-1);
  }
    
  // Calculate refraction
  // See: http://www.flipcode.com/archives/reflection_transmission.pdf
  if (texture->finish->refraction > 0.0) {
    Vector refr_dir;
    double refr_index = texture->finish->refraction_index;
    double n = ray.refraction_index / refr_index;
    Normal normal = intersection.geometry.normal;

    double cosI = -Dot(normal, ray.direction);
    double cosT2 = 1.0 - n * n * (1.0 - cosI * cosI);

    if (cosT2 > 0.0) {
      refr_dir = n * ray.direction + (n * cosI - sqrt(cosT2)) * normal;

      Ray refr_ray = Ray(ipoint + refr_dir * EPS, refr_dir);
      refr_ray.refraction_index = refr_index;
      Intersection refr_inter;
      bool hit;
      Color refr_color = ColorFromRay(scene, kdtree, refr_ray, depth-1,
        &refr_inter, &hit);
      Color absorbed = color::white
                       * (hit ? (texture->finish->absorption
                                 * -refr_inter.distance)
                              : 0.0);                  
      color += texture->finish->refraction * refr_color *
         Color(exp(absorbed.r),
               exp(absorbed.g),
               exp(absorbed.b));
    }
  }

  return Clamp(color);
}
}

Color RayTracePixel(Scene const& scene, KdTree const& kdtree, int width,
  int height, int x, int y, int max_depth, int samples)
{
  Color color = color::black;

  // Supersampling
  for (int sx = 0; sx < samples; ++sx) {
    for (int sy = 0; sy < samples; ++sy) {
      double px = (x + (double)sx / samples) / width;
      double py = (y + (double)sy / samples) / height;

      Ray ray = scene.camera->ray_at(px, py);
      color += ColorFromRay(scene, kdtree, ray, max_depth)
        / (samples * samples);
    }
  }

  return color;
}

}