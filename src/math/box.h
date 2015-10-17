//
//  box.h
//  Bounding boxes
//

#ifndef _RAYTRACE_MATH_BOX_H_
#define _RAYTRACE_MATH_BOX_H_

#include <algorithm>
#include "math/point.h"
#include "math/ray.h"
#include "common.h"

namespace raytrace {;

class Box {
public:
  Point ll;
  Point ur;

  double Width() const { return ur.x - ll.x; }
  double Height() const { return ur.y - ll.y; }
  double Depth() const { return ur.z - ll.z; }

  bool Intersect(Ray const& ray, double *near, double *far) const;

  Box(Point p) : ll(p), ur(p) { }
  Box(Point p1, Point p2);
  Box() : ll(Point(INFINITY, INFINITY, INFINITY)),
          ur(Point(-INFINITY, -INFINITY, -INFINITY))
  { }
  Box(Box const& box) : ll(box.ll), ur(box.ur) { }
};

Box AddPoint(Box const& b, Point const& p);
Box Union(Box const& a, Box const& b);
bool Contains(Box const& b, Point const& p);
bool Overlaps(Box const& b1, Box const& b2);
int LongestSide(Box const& b);
double SurfaceArea(Box const& b);

}

#endif
