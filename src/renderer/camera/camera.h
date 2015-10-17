#ifndef _RAYTRACE_RENDERER_CAMERA_CAMERA_H_
#define _RAYTRACE_RENDERER_CAMERA_CAMERA_H_

#include "common.h"
#include "math/math.h"

namespace raytrace {;

struct Camera {
  virtual Ray ray_at(double u, double v) const {
    return Ray(Point(0.0, 0.0, 0.0), direction::in);
  };

  Vector direction;
  Vector up;
  Vector right;
  Transform transformation;
};

}

#endif