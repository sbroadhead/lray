#include "common.h"
#include "math/vector.h"

#include <cmath>

namespace raytrace {;

Vector operator+(Vector const& v, Vector const& w) {
  Vector u = v;
  u += w;
  return u;
}

Vector operator-(Vector const& v, Vector const& w) {
  Vector u = v;
  u -= w;
  return u;
}

Vector operator*(Vector const& v, double k) {
  Vector u = v;
  u *= k;
  return u;
}

Vector operator*(double k, Vector const& v) {
  Vector u = v;
  u *= k;
  return u;
}

Vector operator/(Vector const& v, double k) {
  Vector u = v;
  u /= k;
  return u;
}

double Dot(Vector const& v, Vector const& w) {
  return v.x * w.x + v.y * w.y + v.z * w.z;
}

Vector Cross(Vector const& v, Vector const& w) {
  Vector cross = Vector(
    v.y * w.z - v.z * w.y,
    v.z * w.x - v.x * w.z,
    v.x * w.y - v.y * w.x);
  return cross;
}

double NormSquared(Vector const& v) {
  return Dot(v, v);
}

double Norm(Vector const& v) {
  return sqrt(NormSquared(v));
}

Vector Normalized(Vector const& v) { 
  return v / Norm(v);
}

bool IsZero(Vector const& v) {
  return v.x == 0 && v.y == 0 && v.z == 0;
}

}