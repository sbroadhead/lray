#include <string>
#include <cstdio>
#include "common.h"
#include "image/image.h"
#include "image/targa.h"
#include "math/math.h"

namespace raytrace { namespace imaging {;

void write_targa(std::string const& filename, Image const& image) {
    FILE *fptr;
    fptr = fopen(filename.c_str(), "wb");
    
    if (!fptr) {
      Error("Failed to open file: %s\n", filename);
    }

    putc(0,fptr);
    putc(0,fptr);
    putc(2,fptr);
    putc(0,fptr); putc(0,fptr);
    putc(0,fptr); putc(0,fptr);
    putc(0,fptr);
    putc(0,fptr); putc(0,fptr);
    putc(0,fptr); putc(0,fptr);
    putc((image.width() & 0x00FF),fptr);
    putc((image.width() & 0xFF00) >> 8,fptr);
    putc((image.height() & 0x00FF),fptr);
    putc((image.height() & 0xFF00) >> 8,fptr);
    putc(24,fptr);
    putc(0,fptr);
    
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            Color color = image.get_pixel(x, y);
            putc((int)(color.b*255),fptr);
            putc((int)(color.g*255),fptr);
            putc((int)(color.r*255),fptr);
        }
    }
    
    fclose(fptr);
}

boost::shared_ptr<Image> read_targa(std::string const& filename) {
  FILE *file = fopen(filename.c_str(), "rb");
  unsigned char ident[12];
  unsigned char tga[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0 };
  unsigned char header[6];
  unsigned char *data;
  int width, height;
    
  if (!file) {
    Error("Failed to open file: %s\n", filename);
  }
    
  fread(ident, 1, sizeof(ident), file);
  if (memcmp(ident, tga, sizeof(tga)) != 0) {
    Error("File is not an uncompressed TGA: %s", filename);
  }
    
  fread(header, 1, sizeof(header), file);
    
  width = (header[1] << 8) + header[0];
  height = (header[3] << 8) + header[2];
    
  if (header[4] != 24 && header[4] != 32) {
    Error("File is not a 24-bit or 32-bit TGA: %s", filename);
  }
    
  int bytes_per_pixel = header[4] >> 3;
  int num_bytes = width * height * bytes_per_pixel;
  data = (unsigned char *)malloc(num_bytes);
  fread(data, sizeof(unsigned char), num_bytes, file);
  fclose(file);

  boost::shared_ptr<Image> image = boost::make_shared<Image>(width, height);

  for (int i = 0; i < num_bytes; i += bytes_per_pixel) {
    int x = (i / bytes_per_pixel) % width;
    int y = (i / bytes_per_pixel) / width;
    image->set_pixel(x, y, Color(data[i+2]/255.0, data[i+1]/255.0, data[i]/255.0));
  }
  
  return image;
}

}}
