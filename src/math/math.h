#ifndef _RAYTRACE_MATH_MATH_H_
#define _RAYTRACE_MATH_MATH_H_

#include <algorithm>

#include "math/color.h"
#include "math/vector.h"
#include "math/point.h"
#include "math/box.h"
#include "math/matrix.h"
#include "math/ray.h"
#include "math/transform.h"

namespace raytrace {;

static bool quadratic(double a, double b, double c, double *y0, double *y1) {
  double disc = b * b - 4 * a * c;
  if (disc <= 0.0) return false;
  double sqrt_disc = sqrt(disc);
  double sgn = b < 0 ? -1 : 1;
  double d = -0.5 * (b + sgn * sqrt_disc);
  *y0 = d / a;
  *y1 = c / d;
  if (*y0 > *y1) std::swap(*y0, *y1);
  return true;
}

static double clamp(double a, double min, double max) {
  return a < min ? min : (a > max ? max : a);
}

namespace direction {;

static const Vector right(1.0, 0.0, 0.0);
static const Vector up(0.0, 1.0, 0.0);
static const Vector in(0.0, 0.0, 1.0);
static const Vector left = -right;
static const Vector down = -up;
static const Vector out = -in;

}

namespace color {;

static const Color red(1.0, 0.0, 0.0);
static const Color green(0.0, 1.0, 0.0);
static const Color blue(0.0, 0.0, 1.0);
static const Color black(0.0, 0.0, 0.0);
static const Color white(1.0, 1.0, 1.0);

static Color grey(double value) {
  return Color(value, value, value);
}

}

}

#endif