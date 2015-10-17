#ifndef _RAYTRACE_RENDERER_TEXTURE_FINISH_H_
#define _RAYTRACE_RENDERER_TEXTURE_FINISH_H_

#include "math/math.h"
#include "common.h"

namespace raytrace {;

struct Finish {
  Color ambient;
  double diffuse;
  double specular;
  double exponent;
  double reflection;
  double refraction;
  double refraction_index;
  double absorption;

  Finish()
      : ambient(0.02, 0.02, 0.02),
        diffuse(0.6),
        specular(0.1),
        exponent(1.0),
        reflection(0.0),
        refraction(0.0),
        refraction_index(1.0),
        absorption(0.5)
  { }
};
//////////////////////////////////////////////////////////////////////////

}

#endif