#ifndef _RAYTRACE_RENDERER_TEXTURE_IMAGEPIGMENT_H_
#define _RAYTRACE_RENDERER_TEXTURE_IMAGEPIGMENT_H_

#include "common.h"
#include "math/math.h"
#include "image/image.h"
#include "image/targa.h"
#include "renderer/texture/texturemap.h"
#include "renderer/texture/pigment.h"

namespace raytrace {;

class ImagePigment2D : public Pigment {
public:
  virtual Color color_at(EntityGeometry const& geometry) const {
    double s, t;
    texture_map->Map(geometry, &s, &t);

    s = s - floor(s);
    t = t - floor(t);
    int x = (int)clamp(s * image->width(), 0, image->width() - 1);
    int y = (int)clamp(t * image->height(), 0, image->height() - 1);

    return image->get_pixel(x, y);
  } 

  boost::shared_ptr<TextureMap2D> texture_map;
  boost::shared_ptr<imaging::Image> image;

  ImagePigment2D(std::string const& filename)
      : texture_map(boost::make_shared<TextureMap2D>())
  {
    image = imaging::read_targa(filename);
  }
        
};

}

#endif