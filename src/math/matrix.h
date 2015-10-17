#ifndef _RAYTRACE_MATH_MATRIX_H_
#define _RAYTRACE_MATH_MATRIX_H_

namespace raytrace {;

class Matrix44 {
public:
  double a[4][4];

  bool operator==(Matrix44 const& other) const {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        if (a[i][j] != other.a[i][j]) {
          return false;
        }
      }
    }

    return true;
  }
  
  bool operator!=(Matrix44 const& other) const {
    return !(*this == other);
  }

  Matrix44(double m11, double m12, double m13, double m14,
           double m21, double m22, double m23, double m24,
           double m31, double m32, double m33, double m34,
           double m41, double m42, double m43, double m44);

  Matrix44();
};

Matrix44 Mult(Matrix44 const& l, Matrix44 const& r);
Matrix44 Transpose(Matrix44 const& m);
Matrix44 Inverse(Matrix44 const& m);

}

#endif