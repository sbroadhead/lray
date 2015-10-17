#ifndef _RAYTRACE_RENDERER_INTERSECTION_H_
#define _RAYTRACE_RENDERER_INTERSECTION_H_

#include "math/math.h"
#include "renderer/geometry.h"

namespace raytrace {;

class Entity;

class Intersection {
public:
  const Entity *entity;
  double distance;
  EntityGeometry geometry;
};

}

#endif