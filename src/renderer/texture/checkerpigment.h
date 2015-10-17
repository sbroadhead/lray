#ifndef _RAYTRACE_RENDERER_TEXTURE_CHECKERPIGMENT_H_
#define _RAYTRACE_RENDERER_TEXTURE_CHECKERPIGMENT_H_

#include "common.h"
#include "math/math.h"
#include "renderer/texture/texturemap.h"
#include "renderer/texture/pigment.h"

namespace raytrace {;

class CheckerPigment2D : public Pigment {
public:
  virtual Color color_at(EntityGeometry const& geometry) const {
    double s, t;
    texture_map->Map(geometry, &s, &t);

    s = s - floor(s);
    t = t - floor(t);

    if ((s > 0.5) == (t > 0.5))
      return pigment1->color_at(geometry);
    else
      return pigment2->color_at(geometry);
  }

  boost::shared_ptr<TextureMap2D> texture_map;
  boost::shared_ptr<Pigment> pigment1;
  boost::shared_ptr<Pigment> pigment2;

  CheckerPigment2D()
      : texture_map(boost::make_shared<TextureMap2D>()),
        pigment1(boost::make_shared<Pigment>()),
        pigment2(boost::make_shared<Pigment>())
  { }
        
};

class CheckerPigment3D : public Pigment {
public:
  virtual Color color_at(EntityGeometry const& geometry) const {
    Point p = texture_map->Map(geometry);

    if ((int(floor(p.x)) + int(floor(p.y)) + int(floor(p.z))) % 2 == 0)
      return pigment1->color_at(geometry);
    else
      return pigment2->color_at(geometry);
  }

  boost::shared_ptr<TextureMap3D> texture_map;
  boost::shared_ptr<Pigment> pigment1;
  boost::shared_ptr<Pigment> pigment2;

  CheckerPigment3D()
      : texture_map(boost::make_shared<TextureMap3D>()),
        pigment1(boost::make_shared<Pigment>()),
        pigment2(boost::make_shared<Pigment>())
  { }
        
};

}

#endif