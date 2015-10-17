#include <algorithm>
#include "math/box.h"

namespace raytrace {;

bool Box::Intersect(Ray const& ray, double *near, double *far) const {
  double t0 = ray.min_dist, t1 = ray.max_dist;
  for (int i = 0; i < 3; ++i) {
    double near_dist = (ll[i] - ray.origin[i]) / ray.direction[i];
    double far_dist = (ur[i] - ray.origin[i]) / ray.direction[i];

    if (near_dist > far_dist) std::swap(near_dist, far_dist);

    t0 = std::max(near_dist, t0);
    t1 = std::min(far_dist, t1);

    if (t0 > t1) return false;
  }

  if (near) *near = t0;
  if (far) *far = t1;
  return true;
}

Box AddPoint(Box const& b, Point const& p) {
  Box r = b;
  r.ll.x = std::min(b.ll.x, p.x);
  r.ll.y = std::min(b.ll.y, p.y);
  r.ll.z = std::min(b.ll.z, p.z);
  r.ur.x = std::max(b.ur.x, p.x);
  r.ur.y = std::max(b.ur.y, p.y);
  r.ur.z = std::max(b.ur.z, p.z);
  return r;
}

Box Union(Box const& a, Box const& b) {
  Box r = a;
  r = AddPoint(r, b.ll);
  r = AddPoint(r, b.ur);
  return r;
}

bool Contains(Box const& b, Point const& p) {
  return (p.x >= b.ll.x && p.x <= b.ur.x &&
          p.y >= b.ll.y && p.y <= b.ur.y &&
          p.z >= b.ll.z && p.z <= b.ur.z);
}

bool Overlaps(Box const& b1, Box const& b2) {
  return (b1.ur.x >= b2.ll.x && b1.ll.x <= b2.ur.x &&
          b1.ur.y >= b2.ll.y && b1.ll.y <= b2.ur.y &&
          b1.ur.z >= b2.ll.z && b1.ll.z <= b2.ur.z);
}

int LongestSide(Box const& b) {
  Vector delta = b.ur - b.ll;

  if (delta.x > delta.y && delta.x > delta.z) {
    return 0;
  } else if (delta.y > delta.z) {
    return 1;
  }

  return 2;
}

double SurfaceArea(Box const& b) {
  return 2.0 * (b.Width() * b.Height() + b.Width() * b.Depth() +
                b.Height() * b.Depth());
}

Box::Box(Point p1, Point p2) {
  ll = p1;
  ur = p2;
}

}