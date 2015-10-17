#ifndef _RAYTRACE_RENDERER_ENTITY_SPHERE_
#define _RAYTRACE_RENDERER_ENTITY_SPHERE_

#include "common.h"
#include "renderer/entity/entity.h"
#include "renderer/intersection.h"

namespace raytrace {;

class SphereEntity : public Entity {
public:
  virtual bool Intersect(Ray const& ray, Intersection *out) const;
  virtual Box BoundingBox() const;

  SphereEntity(Transform const& transformation = Transform())
      : Entity(transformation)
  { }
};

}

#endif