#ifndef _RAYTRACE_UTIL_RANDOM_H_
#define _RAYTRACE_UTIL_RANDOM_H_

#include <cstdlib>
#include <cmath>

static double rand_double(double min, double max) {
  return (double)rand() / RAND_MAX * (max - min) + min;
}

static int rand_int(int min, int max) {
  return (int)rand_double((double)min, (double)max);
}

#endif