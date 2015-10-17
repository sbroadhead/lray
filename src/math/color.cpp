#include <algorithm>
#include "math/color.h"

namespace raytrace {;

Color operator+(Color const& a, Color const& b) {
  Color c = a;
  c += b;
  return c;
}

Color operator-(Color const& a, Color const& b) {
  Color c = a;

  c -= b;
  return c;
}

Color operator*(Color const& a, Color const& b) {
  Color c = a;
  c *= b;
  return c;
}

Color operator*(Color const& a, double k) {
  Color c = a;
  c *= k;
  return c;
}

Color operator*(double k, Color const& a) {
  Color c = a;
  c *= k;
  return c;
}

Color operator/(Color const& a, Color const& b) {
  Color c = a;
  c /= b;
  return c;
}

Color operator/(Color const& a, double k) {
  Color c = a;
  c /= k;
  return c;
}

Color Clamp(Color const& a) {
  Color b;
  b.r = std::min(std::max(a.r, 0.0), 1.0);
  b.g = std::min(std::max(a.g, 0.0), 1.0);
  b.b = std::min(std::max(a.b, 0.0), 1.0);
  b.a = std::min(std::max(a.a, 0.0), 1.0);

  return b;
}

}