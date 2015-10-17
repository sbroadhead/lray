#include "common.h"
#include "math/math.h"
#include "renderer/intersection.h"
#include "renderer/entity/entity.h"
#include "renderer/entity/cone.h"

namespace raytrace {;

Box ConeEntity::BoundingBox() const {
  Box box(Point(-1.0, 0.0, -1.0),
          Point(1.0, 1.0, 1.0));
  return transformation.Apply(box);
}

bool ConeEntity::Intersect(Ray const& ray, Intersection *out) const
{
  Ray new_ray = transformation.Inverse().Apply(ray);

  double a =
    new_ray.direction.x * new_ray.direction.x +
    new_ray.direction.z * new_ray.direction.z -
    new_ray.direction.y * new_ray.direction.y;
  double b = 2 *
    (new_ray.direction.x * new_ray.origin.x +
    new_ray.direction.z * new_ray.origin.z -
    new_ray.direction.y * (new_ray.origin.y - 1));
  double c =
    new_ray.origin.x * new_ray.origin.x +
    new_ray.origin.z * new_ray.origin.z -
    (new_ray.origin.y - 1) * (new_ray.origin.y - 1);

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

