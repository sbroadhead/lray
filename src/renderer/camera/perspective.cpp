#include "common.h"
#include "renderer/camera/perspective.h"
#include "math/math.h"

namespace raytrace {;

Ray Perspective::ray_at(double u, double v) const {
  Point location = this->transformation.Apply(Point(0, 0, 0));
  double adju = u - 0.5, adjv = v - 0.5;
  Vector offset = adju * this->right + adjv * this->up;
  Vector dir = Normalized(this->transformation.Apply(this->direction + offset));
  Ray ray(location, dir);

  return ray;
}

}