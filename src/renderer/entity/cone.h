#ifndef _RAYTRACE_RENDERER_ENTITY_CONE_H_
#define _RAYTRACE_RENDERER_ENTITY_CONE_H_

#include "common.h"
#include "renderer/entity/entity.h"
#include "renderer/intersection.h"

namespace raytrace {;

class ConeEntity : public Entity {
public:
  virtual bool Intersect(Ray const& ray, Intersection *out) const;
  virtual Box BoundingBox() const;

  ConeEntity(Transform const& transformation = Transform())
      : Entity(transformation)
  { }
};

}

#endif