#ifndef _RAYTRACE_MATH_PERLIN_H_
#define _RAYTRACE_MATH_PERLIN_H_

namespace raytrace {;

double PerlinNoise(double x, double y, double z);
double Turbulence(Point point, double size);

}

#endif