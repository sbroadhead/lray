#ifndef _RAYTRACE_RENDERER_TEXTURE_UVMAP_H_
#define _RAYTRACE_RENDERER_TEXTURE_UVMAP_H_

#include "common.h"
#include "renderer/geometry.h"
#include "renderer/texture/texturemap.h"
#include "math/math.h"

namespace raytrace {;

class UVMap : public TextureMap2D {
public:
  double scale_s, scale_t;
  double translate_s, translate_t;

  virtual void Map(EntityGeometry const& geometry, double *s, double *t) const {
    if (s) {
      *s = geometry.u * scale_s + translate_s;
    }
    if (t) {
      *t = geometry.v * scale_t + translate_t;
    }
  }
};

}

#endif