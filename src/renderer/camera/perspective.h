#ifndef _RAYTRACE_RENDERER_CAMERA_PERSPECTIVE_H_
#define _RAYTRACE_RENDERER_CAMERA_PERSPECTIVE_H_

#include "common.h"
#include "renderer/camera/camera.h"

namespace raytrace {;

struct Perspective : public Camera {
  virtual Ray ray_at(double u, double v) const;
  void set_angle(double angle);

  Perspective(double angle = 45.0, double aspect = (4.0/3.0))
  {
    this->direction = direction::in;
    this->up = direction::up;
    this->right = direction::right * aspect;
    this->direction *= 0.5 * Norm(this->right)
        / tan(DEG_TO_RAD(angle / 2)); // from POV-Ray docs;
  }
};

}

#endif