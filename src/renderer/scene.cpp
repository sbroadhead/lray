#include "common.h"
#include "math/math.h"
#include "renderer/intersection.h"
#include "renderer/scene.h"

namespace raytrace {;

void Scene::AddEntity(boost::shared_ptr<Entity> const& entity) {
  entities.push_back(entity);
}

void Scene::AddLight(boost::shared_ptr<Light> const& light) {
  lights.push_back(light);
}

bool Scene::Intersect(Ray const& ray, Intersection *out) const {
  Intersection nearest;
  nearest.distance = HUGE_VAL;

  for (vector<boost::shared_ptr<Entity> >::const_iterator it = entities.begin();
      it != entities.end(); ++it) {
    Entity const& entity = **it;
    Intersection& inter = nearest;

    Ray new_ray = ray;
    new_ray.max_dist = nearest.distance;
    bool did_hit = entity.Intersect(new_ray, out ? &nearest : NULL);

    if (did_hit) {
      if (!out) {
        return true;
      }

      if (out && inter.distance < nearest.distance) {
        nearest = inter;
      }
    }
  }

  if (nearest.distance == HUGE_VAL) {
    return false;
  }

  if (out) *out = nearest;
  return true;
};

}