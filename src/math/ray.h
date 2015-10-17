#ifndef _RAYTRACE_MATH_RAY_H_
#define _RAYTRACE_MATH_RAY_H_

#include "common.h"
#include "math/point.h"
#include "math/vector.h"

namespace raytrace {;

class Ray {
public:
  Point origin;
  Vector direction;
  double min_dist; // Ignore intersections less than min_dist
  double max_dist; // or greater than max_dist
  double refraction_index; // Refraction_index of the medium where the ray
                           // was spawned

  // Evaluate the ray at a point
  Point operator()(double t) const {
    return origin + t * direction;
  }

  Ray(Point const& origin = Point(0.0, 0.0, 0.0),
      Vector const& direction = Vector(0.0, 0.0, 0.0),
      int depth = 0)
      : origin(origin),
        direction(direction),
        min_dist(0.0),
        max_dist(INFINITY),
        refraction_index(1.0)
  { }
};

}

#endif