#ifndef _RAYTRACE_MATH_COLOR_H_
#define _RAYTRACE_MATH_COLOR_H_

namespace raytrace {;

class Color {
public:
  double r, g, b, a;

  Color& operator+=(Color const& other) {
    r += other.r;
    g += other.g;
    b += other.b;
    a += other.a;

    return *this;
  }

  Color& operator-=(Color const& other) {
    r -= other.r;
    g -= other.g;
    b -= other.b;
    a -= other.a;

    return *this;
  }

  Color& operator*=(Color const& other) {
    r *= other.r;
    g *= other.g;
    b *= other.b;
    a *= other.a;

    return *this;
  }

  Color& operator*=(double k) {
    r *= k;
    g *= k;
    b *= k;
    a *= k;

    return *this;
  }

  Color& operator/=(Color const& other) {
    r /= other.r;
    g /= other.g;
    b /= other.b;
    a /= other.a;
    
    return *this;
  }

  Color& operator/=(double k) {
    r /= k;
    g /= k;
    b /= k;
    a /= k;

    return *this;
  }

  bool operator==(Color const& other) {
    return r == other.r && g == other.g && b == other.b && a == other.a;
  }

  bool operator!=(Color const& other) {
    return !(*this == other);
  }

  explicit Color(double r = 0.0, double g = 0.0, double b = 0.0)
      : r(r), g(g), b(b)
  { }
};

Color operator+(Color const& a, Color const& b);
Color operator-(Color const& a, Color const& b);
Color operator*(Color const& a, Color const& b);
Color operator*(Color const& a, double k);
Color operator*(double k, Color const& a);
Color operator/(Color const& a, Color const& b);
Color operator/(Color const& a, double k);

Color Clamp(Color const& a);
#endif

}