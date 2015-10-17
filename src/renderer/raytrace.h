#ifndef _RAYTRACE_RENDERER_RAYTRACE_H_
#define _RAYTRACE_RENDERER_RAYTRACE_H_

#include "common.h"
#include "math/math.h"
#include "renderer/scene.h"
#include "renderer/accel/kdtree.h"

namespace raytrace {;

struct SceneInfo {
  int image_width;
  int image_height;
  int samples;
  int frames;
  double start_clock;
  double end_clock;

  SceneInfo()
      : image_width(800), image_height(600), samples(1), frames(1),
        start_clock(0.0), end_clock(1.0)
  { }
};

typedef void (*raytrace_callback)(int x, int y, Color const& color);

void RayTrace(Scene const& scene, int width, int height,
              raytrace_callback callback, int max_depth = 3, int samples = 2);

Color RayTracePixel(Scene const& scene, KdTree const& kdtree, int width,
  int height, int x, int y, int max_depth, int samples);

}

#endif