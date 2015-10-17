#ifndef _RAYTRACE_IMAGE_IMAGE_H_
#define _RAYTRACE_IMAGE_IMAGE_H_

#include "common.h"
#include "math/math.h"

namespace raytrace { namespace imaging {;

class Image {
public:
    void set_pixel(int x, int y, Color const& color)
      { pixels_[y * width_ + x] = color; }
    Color const& get_pixel(int x, int y) const
      { return pixels_[y * width_ + x]; } 
    
    inline int width() const { return width_; }
    inline int height() const { return height_; }
    
    Image(int width, int height)
        : width_(width),
          height_(height),
          pixels_(new Color[width * height]) { }

    ~Image() {
      if (pixels_) delete [] pixels_;
    }
private:
    int width_;
    int height_;
    Color *pixels_;
};

}}

#endif