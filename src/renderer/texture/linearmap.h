#ifndef _RAYTRACE_RENDERER_TEXTURE_LINEARMAP_H_
#define _RAYTRACE_RENDERER_TEXTURE_LINEARMAP_H_

#include "common.h"
#include "renderer/geometry.h"
#include "renderer/texture/texturemap.h"
#include "math/math.h"

namespace raytrace {;

class LinearMap : public TextureMap3D {
public:
  Transform transform;

  virtual Point Map(EntityGeometry const& geometry) const {
    return transform.Apply(geometry.point_local);
  }
};

}

#endif