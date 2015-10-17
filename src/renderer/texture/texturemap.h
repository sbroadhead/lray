#ifndef _RAYTRACE_RENDERER_TEXTURE_TEXTUREMAP_H_
#define _RAYTRACE_RENDERER_TEXTURE_TEXTUREMAP_H_

#include "common.h"
#include "renderer/geometry.h"
#include "math/math.h"

namespace raytrace {;

class TextureMap2D {
public:
  virtual void Map(EntityGeometry const& geometry, double *s, double *t) const {
    if (s) *s = geometry.u;
    if (t) *t = geometry.v;
  };
};

class TextureMap3D {
public:
  virtual Point Map(EntityGeometry const& geometry) const {
    return geometry.point;
  }
};

}

#endif