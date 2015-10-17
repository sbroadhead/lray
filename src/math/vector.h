#ifndef _RAYTRACE_MATH_VECTOR_H_
#define _RAYTRACE_MATH_VECTOR_H_

#include <cmath>

namespace raytrace {;

class Vector {
public:
  double x, y, z;

  Vector operator-() const {
    Vector negated = Vector(-x, -y, -z);
    return negated;
  }

  Vector& operator+=(Vector const& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  Vector& operator-=(Vector const& other) {
    *this += -other;
    return *this;
  }

  Vector& operator*=(double other) {
    x *= other;
    y *= other;
    z *= other;
    return *this;
 }

  Vector& operator/=(double other) {
    *this *= 1.0/other;
    return *this;
  }

  bool operator==(Vector const& other) {
    return x == other.x && y == other.y && z == other.z;
  }
  
  bool operator!=(Vector const& other) {
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

  Vector(double x, double y, double z) : x(x), y(y), z(z) { }

  Vector() : x(0.0), y(0.0), z(0.0) { }
};

Vector operator+(Vector const& v, Vector const& w);
Vector operator-(Vector const& v, Vector const& w);
Vector operator*(Vector const& v, double k);
Vector operator*(double k, Vector const& v);
Vector operator/(Vector const& v, double k);
double Dot(Vector const& v, Vector const& w);
Vector Cross(Vector const& v, Vector const& w);
double NormSquared(Vector const& v);
double Norm(Vector const& v);
Vector Normalized(Vector const& v);
bool IsZero(Vector const& v);

// Represents a normal vector in Euclidian space. This is identical to a
// regular vector, but it has its own type so that the Transform class can
// apply transformations to it in a special way.
class Normal : public Vector {
public:
  Normal(double x, double y, double z) : Vector(x, y, z) { }

  Normal(Vector const& v) : Vector(v.x, v.y, v.z) { }

  Normal() : Vector() { }
};

}

#endif