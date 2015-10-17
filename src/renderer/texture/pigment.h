#ifndef _RAYTRACE_RENDERER_TEXTURE_PIGMENT_H_
#define _RAYTRACE_RENDERER_TEXTURE_PIGMENT_H_

#include "renderer/geometry.h"
#include "common.h"

namespace raytrace {;

class Pigment {
public:
  virtual Color color_at(EntityGeometry const& geometry) const
    { return color::white; }
  virtual ~Pigment() { };
};

}

#endif
