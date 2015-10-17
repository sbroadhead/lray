#ifndef _RAYTRACE_RENDERER_TEXTURE_COLORPIGMENT_H_
#define _RAYTRACE_RENDERER_TEXTURE_COLORPIGMENT_H_

#include "common.h"
#include "renderer/texture/pigment.h"

namespace raytrace {;

class ColorPigment : public Pigment {
public:
  virtual Color color_at(EntityGeometry const& geometry) const {
    return color;
  }

  Color color;

  ColorPigment(Color color = color::white) : color(color)
  { }
};

}

#endif