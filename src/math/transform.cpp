#include "common.h"
#include "math/math.h"

namespace raytrace {;

Transform operator*(Transform const& s, Transform const& t) {
  Transform r = s;
  r *= t;
  return r;
}

Transform Translate(Vector const& v) {
  Matrix44 mat = Matrix44(
    1.0, 0.0, 0.0, v.x,
    0.0, 1.0, 0.0, v.y,
    0.0, 0.0, 1.0, v.z,
    0.0, 0.0, 0.0, 1.0);
  Matrix44 matinv = Matrix44(
    1.0, 0.0, 0.0, -v.x,
    0.0, 1.0, 0.0, -v.y,
    0.0, 0.0, 1.0, -v.z,
    0.0, 0.0, 0.0, 1.0
  );

  return Transform(mat, matinv);
}

Transform Scale(double x, double y, double z) {
  Matrix44 mat = Matrix44(
    x, 0.0, 0.0, 0.0,
    0.0, y, 0.0, 0.0,
    0.0, 0.0, z, 0.0,
    0.0, 0.0, 0.0, 1.0);
  Matrix44 matinv = Matrix44(
    1.0/x, 0.0, 0.0, 0.0,
    0.0, 1.0/y, 0.0, 0.0,
    0.0, 0.0, 1.0/z, 0.0,
    0.0, 0.0, 0.0, 1.0);

  return Transform(mat, matinv);
}

Transform Rotate(Vector const& v, double t) {
  double cos_t = cos(DEG_TO_RAD(t));
  double sin_t = sin(DEG_TO_RAD(t));
  Matrix44 mat;

  mat.a[0][0] = (v.x * v.x) + (1 - v.x * v.x) * cos_t;
  mat.a[0][1] = (v.x * v.y) * (1 - cos_t) - v.z * sin_t;
  mat.a[0][2] = (v.x * v.z) * (1 - cos_t) + v.y * sin_t;
  mat.a[0][3] = 0.0;

  mat.a[1][0] = (v.x * v.y) * (1 - cos_t) + v.z * sin_t;
  mat.a[1][1] = (v.y * v.y) + (1 - v.y * v.y) * cos_t;
  mat.a[1][2] = (v.y * v.z) * (1 - cos_t) - v.x * sin_t;
  mat.a[1][3] = 0.0;

  mat.a[2][0] = (v.x * v.z) * (1 - cos_t) - v.y * sin_t;
  mat.a[2][1] = (v.y * v.z) * (1 - cos_t) + v.x * sin_t;
  mat.a[2][2] = (v.z * v.z) + (1 - v.z * v.z) * cos_t;
  mat.a[2][3] = 0.0;

  mat.a[3][0] = 0.0;
  mat.a[3][1] = 0.0;
  mat.a[3][2] = 0.0;
  mat.a[3][3] = 1.0;

  // Rotations are orthonormal, so transpose(A) = inverse(A)
  return Transform(mat, Transpose(mat));
}

//
// A look-at matrix is a change of basis matrix which changes from the standard
// basis to some rotated basis, and then translates to some world position.
//
Transform LookAt(Vector const& eye, Vector const& at, Vector const& up) {
  Matrix44 mat;
  Vector zprime = Normalized(at - eye);
  Vector xprime = Normalized(Cross(up, zprime));
  Vector yprime = Cross(zprime, xprime);

  mat.a[0][0] = xprime.x;
  mat.a[1][0] = xprime.y;
  mat.a[2][0] = xprime.z;
  mat.a[3][0] = 0.0;

  mat.a[0][1] = yprime.x;
  mat.a[1][1] = yprime.y;
  mat.a[2][1] = yprime.z;
  mat.a[3][1] = 0.0;

  mat.a[0][2] = zprime.x;
  mat.a[1][2] = zprime.y;
  mat.a[2][2] = zprime.z;
  mat.a[3][2] = 0.0;

  mat.a[0][3] = eye.x;
  mat.a[1][3] = eye.y;
  mat.a[2][3] = eye.z;
  mat.a[3][3] = 1.0;

  return Transform(mat, Inverse(mat));
}

Vector Transform::Apply(Vector const& v) const {
  Vector w = Vector(
    v.x * mat.a[0][0] + v.y * mat.a[0][1] + v.z * mat.a[0][2],
    v.x * mat.a[1][0] + v.y * mat.a[1][1] + v.z * mat.a[1][2],
    v.x * mat.a[2][0] + v.y * mat.a[2][1] + v.z * mat.a[2][2]);

  return w;
}

Point Transform::Apply(Point const& p) const {
  Point q = Point(
    p.x * mat.a[0][0] + p.y * mat.a[0][1] + p.z * mat.a[0][2] + mat.a[0][3],
    p.x * mat.a[1][0] + p.y * mat.a[1][1] + p.z * mat.a[1][2] + mat.a[1][3],
    p.x * mat.a[2][0] + p.y * mat.a[2][1] + p.z * mat.a[2][2] + mat.a[2][3]);

  return q;
}

Normal Transform::Apply(Normal const& v) const {
  Normal n = Normal(
    v.x * matinv.a[0][0] + v.y * matinv.a[1][0] + v.z * matinv.a[2][0],
    v.x * matinv.a[0][1] + v.y * matinv.a[1][1] + v.z * matinv.a[2][1],
    v.x * matinv.a[0][2] + v.y * matinv.a[1][2] + v.z * matinv.a[2][2]);
  return n;
}

Ray Transform::Apply(Ray const& r) const {
  Ray ray = r;
  ray.origin = Apply(r.origin);
  ray.direction = Apply(r.direction);
  return ray;
}

// Boxes are transformed by transforming each individual point and adding it to 
// a new box.
Box Transform::Apply(Box const& b) const {
  Box r(Apply(Point(b.ll.x, b.ll.y, b.ll.z)));
  r = AddPoint(r, Apply(Point(b.ll.x, b.ll.y, b.ur.z)));
  r = AddPoint(r, Apply(Point(b.ll.x, b.ur.y, b.ll.z)));
  r = AddPoint(r, Apply(Point(b.ll.x, b.ur.y, b.ur.z)));
  r = AddPoint(r, Apply(Point(b.ur.x, b.ll.y, b.ll.z)));
  r = AddPoint(r, Apply(Point(b.ur.x, b.ll.y, b.ur.z)));
  r = AddPoint(r, Apply(Point(b.ur.x, b.ur.y, b.ll.z)));
  r = AddPoint(r, Apply(Point(b.ur.x, b.ur.y, b.ur.z)));
  return r;
}

}
