#ifndef _RAYTRACE_LIGHT_H_
#define _RAYTRACE_LIGHT_H_

#include "common.h"
#include "math/math.h"
#include "renderer/entity/entity.h"
#include "renderer/intersection.h"

namespace raytrace {;

struct Light {
public:
  Color color;
  bool area;
  int rows;
  int cols;
  Transform transformation;
  Color Sample(Point const& point, Ray *out_ray, int row, int col) const;

  Light()
      : color(color::white),
        rows(1), cols(1), area(false)
  { }

};

}

#endif