#ifndef _RAYTRACE_ENTITY_ENTITY_H_
#define _RAYTRACE_ENTITY_ENTITY_H_

#include "common.h"
#include "renderer/intersection.h"
#include "renderer/texture/texture.h"
#include "math/math.h"

namespace raytrace {;

class Entity {
public:
  virtual bool Intersect(Ray const& ray, Intersection *out) const = 0;
  virtual Box BoundingBox() const = 0;

  boost::shared_ptr<Texture> texture;

  Entity(Transform const& transformation = Transform())
      : transformation(transformation),
        texture(boost::make_shared<Texture>())
  { }

  virtual ~Entity() { }
protected:
  Transform transformation;
};

}

#endif
