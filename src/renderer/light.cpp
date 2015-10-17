#include "util/random.h"
#include "renderer/light.h"
#include "math/math.h"

namespace raytrace {;

Color Light::Sample(Point const& point, Ray *out_ray, int row, int col) const {
  Point sample_point(0.0, 0.0, 0.0);
  double cell_width = 1.0 / cols;
  double cell_height = 1.0 / rows;

  if (area) {
    double offx = rand_double(-0.25, 0.25) / cols;
    double offy = rand_double(-0.25, 0.25) / rows;
    double x = (col * cell_width + offx) - 0.5;
    double y = (row * cell_height + offy) - 0.5;
    sample_point = Point(x, 0.0, y);
  }

  sample_point = transformation.Apply(sample_point);
  if (out_ray) {
    *out_ray = Ray(point, Normalized(sample_point - point));
    (*out_ray).max_dist = Norm(sample_point - point);
  }

  // In future, this may be deferred to a subclass for e.g. multicolored lights
  return color;
}

}