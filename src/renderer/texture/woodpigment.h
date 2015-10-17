#ifndef _RAYTRACE_RENDERER_TEXTURE_WOODPIGMENT_H_
#define _RAYTRACE_RENDERER_TEXTURE_WOODPIGMENT_H_

#include "common.h"
#include "math/math.h"
#include "math/perlin.h"
#include "renderer/texture/texturemap.h"
#include "renderer/texture/pigment.h"

namespace raytrace {;

class WoodPigment3D : public Pigment {
public:
  virtual Color color_at(EntityGeometry const& geometry) const {
    Point p = texture_map->Map(geometry);

    double t = sqrt(frequency.x * p.x * p.x + frequency.y * p.y * p.y)
      + power * Turbulence(p, size);
    double amplitude = fabs(sin(t));

    return amplitude * color1 + (1 - amplitude) * color2;
  } 

  boost::shared_ptr<TextureMap3D> texture_map;
  Color color1;
  Color color2;
  Vector frequency;
  double power;
  double size;

  WoodPigment3D()
      : texture_map(boost::make_shared<TextureMap3D>()),
        color1(color::black),
        color2(color::white),
        frequency(Vector(1024, 1024, 1024)),
        power(2.0),
        size(0.2)
  { }
        
};

}

#endif