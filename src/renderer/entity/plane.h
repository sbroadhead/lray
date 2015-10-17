#ifndef _RAYTRACE_RENDERER_ENTITY_PLANE_H_
#define _RAYTRACE_RENDERER_ENTITY_PLANE_H_

#include "common.h"
#include "renderer/entity/entity.h"
#include "renderer/intersection.h"
#include "math/ray.h"

namespace raytrace {;

class PlaneEntity : public Entity {
public:
  virtual bool Intersect(Ray const& ray, Intersection *out) const;
  virtual Box BoundingBox() const;

  PlaneEntity(Transform const& transformation = Transform())
    : Entity(transformation)
  { }
};

}

#endif