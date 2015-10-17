#include "common.h"
#include "renderer/entity/plane.h"
#include "renderer/intersection.h"
#include "math/math.h"

namespace raytrace {;

/**
 */
Box PlaneEntity::BoundingBox() const {
  return Box(Point(-INFINITY, -INFINITY, -INFINITY),
             Point(INFINITY, INFINITY, INFINITY));
}

/**
 */
bool PlaneEntity::Intersect(Ray const& ray, Intersection *out) const
{
  double cos_angle = Dot(ray.direction, direction::up);
  Ray new_ray = transformation.Inverse().Apply(ray);

  if (fabs(cos_angle) > EPS) {
    Vector ray_diff = Point(0, 0, 0) - new_ray.origin;
    double dist = Dot(ray_diff, direction::up) / cos_angle;
    if (dist >= new_ray.min_dist && dist <= new_ray.max_dist) {
      Point point = new_ray.origin + dist * new_ray.direction;

      if (out) {
        out->entity = this;
        out->distance = dist;
        out->geometry.normal = transformation.Apply(Normal(direction::up));
      }

      return true;
    } else {
      int a = 0;
    }
  }

  return false;
}

}