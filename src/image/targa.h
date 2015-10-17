#ifndef _RAYTRACE_IMAGE_TARGA_H_
#define _RAYTRACE_IMAGE_TARGA_H_

#include "common.h"
#include "image/image.h"

namespace raytrace { namespace imaging {;

void write_targa(std::string const& filename, Image const& image);
boost::shared_ptr<Image> read_targa(std::string const& filename);

}
}

#endif