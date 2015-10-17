#ifndef _RAYTRACE_MATH_POINT_H_
#define _RAYTRACE_MATH_POINT_H_

#include "math/vector.h"

namespace raytrace {;

class Point {
public:
  double x, y, z;

  Point operator-() const {
    Point negated = Point(-x, -y, -z);
    return negated;
  }

  Point& operator+=(Vector const& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  Point& operator-=(Vector const& other) {
    *this += -other;
    return *this;
  }

  Point& operator*=(double other) {
    x *= other;
    y *= other;
    z *= other;
    return *this;
  }

  Point& operator/=(double other) {
    *this *= 1.0/other;
    return *this;
  }

  bool operator==(Point const& other) {
    return x == other.x && y == other.y && z == other.z;
  }
  
  bool operator!=(Point const& other) {
    return !(*this == other);
  }

  double& operator[](int i) {
    switch (i%3) {
    case 0:
      return x;
    case 1:
      return y;
    default:
      return z;
    }
  }

  double const& operator[](int i) const {
    switch (i%3) {
    case 0:
      return x;
    case 1:
      return y;
    default:
      return z;
    }
  }

  operator Vector() {
    return Vector(x, y, z);
  }

  Point(double x, double y, double z) : x(x), y(y), z(z) { }

  Point() : x(0.0), y(0.0), z(0.0) { }

  Point(Vector const& vector) : x(vector.x), y(vector.y), z(vector.z) { }
};

Point operator+(Point const& p, Vector const& v);
Point operator-(Point const& p, Vector const& v);
Vector operator-(Point const& p, Point const& q);
Point operator*(Point const& p, double k);
Point operator*(double k, Point const& p);
Point operator/(Point const& p, double k);

}

#endif