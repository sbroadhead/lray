#ifndef _RAYTRACE_RENDERER_TEXTURE_TEXTURE_H_
#define _RAYTRACE_RENDERER_TEXTURE_TEXTURE_H_

#include "common.h"
#include "renderer/texture/finish.h"
#include "renderer/texture/pigment.h"
#include "renderer/texture/texturemap.h"

namespace raytrace {;

struct Texture {
  boost::shared_ptr<Finish> finish;
  boost::shared_ptr<Pigment> pigment;

  Texture()
      : finish(boost::make_shared<Finish>()),
        pigment(boost::make_shared<Pigment>())
  { }
};


}

#endif
