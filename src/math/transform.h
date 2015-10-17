#ifndef _RAYTRACE_MATH_TRANSFORM_H_
#define _RAYTRACE_MATH_TRANSFORM_H_

#include "math/transform.h"
#include "math/point.h"
#include "math/vector.h"
#include "math/box.h"
#include "math/ray.h"
#include "math/matrix.h"

namespace raytrace {;

// A Transform is a wrapper for a Matrix which knows its own inverse at
// all times and modifies different types of geometric objects in different
// ways.
class Transform {
public:
  Matrix44 const& Matrix() const {
    return mat;
  }

  Matrix44 const& InverseMatrix() const {
    return matinv;
  }

  Transform Inverse() const {
    return Transform(matinv, mat);
  }

  Transform& operator*=(Transform const& t) {
    mat = Mult(mat, t.mat);
    matinv = Mult(t.matinv, matinv);
    return *this;
  }

  bool operator==(Transform const& other) const {
    return mat == other.mat && matinv == other.matinv;
  }
  
  bool operator!=(Transform const& other) const {
    return !(*this == other);
  }

  Vector Apply(Vector const& v) const;
  Point Apply(Point const& p) const;
  Normal Apply(Normal const& v) const;
  Ray Apply(Ray const& r) const;
  Box Apply(Box const& r) const;

  Transform(Matrix44 const& mat, Matrix44 const& matinv)
      : mat(mat),
        matinv(matinv)
  { }

  Transform(Matrix44 const& mat)
      : mat(mat)
  {
    matinv = raytrace::Inverse(mat);
  }

  Transform()
      : mat(Matrix44()), matinv(Matrix44())
  { }

private:
  Matrix44 mat, matinv;
};


Transform operator*(Transform const& s, Transform const& t);
Transform Translate(Vector const& v);
Transform Scale(double x, double y, double z);
Transform Rotate(Vector const& v, double t);
Transform LookAt(Vector const& eye, Vector const& at, Vector const& up);

}

#endif