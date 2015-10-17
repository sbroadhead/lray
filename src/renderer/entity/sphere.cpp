#include "common.h"
#include "math/math.h"
#include "renderer/intersection.h"
#include "renderer/entity/sphere.h"

namespace raytrace {;

Box SphereEntity::BoundingBox() const {
  Box box(Point(-1.0, -1.0, -1.0),
          Point(1.0, 1.0, 1.0));
  return transformation.Apply(box);
}

bool SphereEntity::Intersect(Ray const& ray, Intersection *out) const
{
  Ray new_ray = transformation.Inverse().Apply(ray);

  double a = NormSquared(new_ray.direction);
  double b = 2 * Dot(new_ray.direction, Vector(new_ray.origin));
  double c = NormSquared(Vector(new_ray.origin)) - 1.0;

  double y0, y1;
  if (!quadratic(a, b, c, &y0, &y1)) {
    return false;
  }

  if (y0 > ray.max_dist || y1 < ray.min_dist) {
    return false;
  }

  bool inside = false;
  double hit_dist = y0;
  if (y0 < ray.min_dist) {
    hit_dist = y1;
    inside = true;
    if (y1 > ray.max_dist) {
      return false;
    }
  }

  if (!out) return true;

  Point point = new_ray(hit_dist);
  Normal normal = Normalized(Vector(point));
  if (point.x == 0.0) { point.x = EPS; } // prevent NaN from occurring

  double u = (atan2(point.z, point.x) + M_PI) / (2 * M_PI);
  double v = (point.y + 1.0) / 2.0;

  out->entity = this;
  out->distance = hit_dist;
  out->geometry.point = ray(hit_dist);
  out->geometry.point_local = new_ray(hit_dist);
  out->geometry.normal = Normalized(transformation.Apply(normal));
  if (inside) out->geometry.normal = -out->geometry.normal;
  out->geometry.u = u;
  out->geometry.v = v;

  return true;
}

}