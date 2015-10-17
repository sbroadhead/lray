#ifndef _RAYTRACE_RENDERER_GEOMETRY_H_
#define _RAYTRACE_RENDERER_GEOMETRY_H_

namespace raytrace {;

struct EntityGeometry {
  Point point;
  Point point_local;
  Normal normal;
  double u, v;
};

}

#endif