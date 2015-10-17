#ifndef _RAYTRACE_COMMON_H_
#define _RAYTRACE_COMMON_H_

#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <string>
#include <cstdarg>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

using std::vector;
using std::string;

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

// Epsilon value for floating point comparisons
#define EPS 0.001

#ifndef INFINITY
#define INFINITY HUGE_VAL
#endif

#ifndef M_PI
#define M_PI 3.141592653589793238462
#endif

#define DEG_TO_RAD(x) ((x)/180.0*M_PI)
#define RAD_TO_DEG(x) ((x)/M_PI*180.0)

static void Error(const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  
  fprintf(stderr, "FATAL ERROR. ");
  vfprintf(stderr, fmt, list);
  fprintf(stderr, "\n\n");
  va_end(list);
  exit(1);
}

#endif
