#include "common.h"
#include "math/math.h"
#include "renderer/intersection.h"
#include "renderer/entity/entity.h"
#include "renderer/entity/cylinder.h"

namespace raytrace {;

Box CylinderEntity::BoundingBox() const {
  Box box(Point(-1.0, 0.0, -1.0),
          Point(1.0, 1.0, 1.0));
  return transformation.Apply(box);
}

bool CylinderEntity::Intersect(Ray const& ray, Intersection *out) const
{
  Ray new_ray = transformation.Inverse().Apply(ray);
  Ray proj_ray = new_ray;
  proj_ray.direction.y = 0.0;
  proj_ray.origin.y = 0.0;
  
  double a = NormSquared(proj_ray.direction);
  double b = 2 * Dot(proj_ray.direction, Vector(proj_ray.origin));
  double c = NormSquared(Vector(proj_ray.origin)) - 1.0;

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

  Point point = new_ray(hit_dist);
  if (point.y < 0.0 || point.y > 1.0) {
    if (hit_dist == y1) {
      return false;
    }

    hit_dist = y1;
    inside = true;

    point = new_ray(hit_dist);
    if (point.y < 0.0 || point.y > 1.0) {
      return false;
    }
  }

  if (!out) return true;

  Normal normal = Vector(point);
  normal.y = 0.0;
  normal = Normalized(normal);

  double angle = atan2(point.z, point.x);
  double u = angle / (2 * M_PI);
  double v = point.y;

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

