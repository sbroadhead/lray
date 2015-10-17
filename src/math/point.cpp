#include "math/point.h"
#include "math/vector.h"

namespace raytrace {;

Point operator+(Point const& p, Vector const& v) {
  Point u = p;
  u += v;
  return u;
}

Point operator-(Point const& p, Vector const& v) {
  Point u = p;
  u -= v;
  return u;
}

Vector operator-(Point const& p, Point const& q) {
  return Vector(p.x - q.x, p.y - q.y, p.z - q.z);
}

Point operator*(Point const& p, double k) {
  Point u = p;
  u *= k;
  return u;
}

Point operator*(double k, Point const& p) {
  Point u = p;
  u *= k;
  return u;
}

Point operator/(Point const& p, double k) {
  Point u = p;
  u /= k;
  return u;
}

}