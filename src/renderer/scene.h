#ifndef _RAYTRACE_RENDERER_SCENE_H_
#define _RAYTRACE_RENDERER_SCENE_H_

#include "common.h"
#include "math/math.h"
#include "renderer/camera/camera.h"
#include "renderer/entity/entity.h"
#include "renderer/light.h"
#include "renderer/accel/kdtree.h"

namespace raytrace {;

using ::std::vector;

class Scene {
public:
  Color background_color;
  boost::shared_ptr<Camera> camera;
  bool Intersect(Ray const& ray, Intersection *out) const;

  void AddEntity(boost::shared_ptr<Entity> const& entity);
  void AddLight(boost::shared_ptr<Light> const& light);

  vector<boost::shared_ptr<Entity> > const& Entities() const { return entities; }
  vector<boost::shared_ptr<Light> > const& Lights() const { return lights; }

  Scene()
    : entities(vector<boost::shared_ptr<Entity> >()),
      lights(vector<boost::shared_ptr<Light> >()),
      background_color(color::black),
      camera(boost::make_shared<Camera>())
  { }
private:
  vector<boost::shared_ptr<Entity> > entities;
  vector<boost::shared_ptr<Light> > lights;
};

}

#endif