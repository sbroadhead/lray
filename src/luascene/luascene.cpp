#include <iostream>
#include <string>
#include <cstdarg>
#include <sstream>
#include <boost/filesystem.hpp>


#include "math/math.h"
#include "renderer/scene.h"
#include "renderer/light.h"
#include "renderer/raytrace.h"

#include "renderer/entity/entity.h"
#include "renderer/entity/plane.h"
#include "renderer/entity/sphere.h"
#include "renderer/entity/cylinder.h"
#include "renderer/entity/cone.h"
#include "renderer/entity/mesh.h"

#include "renderer/texture/checkerpigment.h"
#include "renderer/texture/colorpigment.h"
#include "renderer/texture/marblepigment.h"
#include "renderer/texture/woodpigment.h"
#include "renderer/texture/imagepigment.h"
#include "renderer/texture/finish.h"
#include "renderer/texture/pigment.h"
#include "renderer/texture/texturemap.h"
#include "renderer/texture/uvmap.h"
#include "renderer/texture/linearmap.h"
#include "renderer/texture/texture.h"

#include "renderer/camera/camera.h"
#include "renderer/camera/perspective.h"

#include "luabind/luabind.hpp"
#include "luabind/shared_ptr_converter.hpp"
#include "luabind/operator.hpp"
#include "luabind/class_info.hpp"

#include "luascene/luascene.h"


using namespace std;
using namespace luabind;
namespace fs = boost::filesystem;

namespace raytrace {;

LuaScene::LuaScene() {
  lua = lua_open();
  luaL_openlibs(lua);
  open(lua);
  bind_class_info(lua);
  bind_classes();
}

LuaScene::~LuaScene() {
  lua_close(lua);
}

// From the luabind manual
int add_file_and_line(lua_State* L) {
  lua_Debug d;
  lua_getstack(L, 1, &d);
  lua_getinfo(L, "Sln", &d);
  std::string err = lua_tostring(L, -1);
  lua_pop(L, 1);
  std::stringstream msg;
  msg << d.short_src << ":" << d.currentline;

  if (d.name != 0)
  {
    msg << "(" << d.namewhat << " " << d.name << ")";
  }
  msg << "\n" << err;
  lua_pushstring(L, msg.str().c_str());
  return 1;
}

// Helper function to add meshes from lua, taking tables (Lua arrays) and
// turning them into STL vectors
void lua_add_mesh(Transform const& transformation,
  object const& points, object const& normals, object const& faces,
  object const& uv, bool smooth, boost::shared_ptr<Texture> const& texture,
  Scene& scene)
{
  if (type(points) != LUA_TTABLE || type(faces) != LUA_TTABLE ||
      type(uv) != LUA_TTABLE || type(normals) != LUA_TTABLE) return;

  vector<Point> point_vector;
  vector<Normal> normal_vector;
  vector<UV> uv_vector;
  vector<MeshFace> face_vector;

  for (luabind::iterator i(points), end; i != end; ++i) {
    point_vector.push_back(object_cast<Point>(*i));
  }

  for (luabind::iterator i(faces), end; i != end; ++i) {
    face_vector.push_back(object_cast<MeshFace>(*i));
  }

  for (luabind::iterator i(uv), end; i != end; ++i) {
    uv_vector.push_back(object_cast<UV>(*i));
  }

  for (luabind::iterator i(normals), end; i != end; ++i) {
    normal_vector.push_back(object_cast<Normal>(*i));
  }
  
  ConstructMesh(transformation, point_vector, normal_vector, uv_vector,
                face_vector, smooth, texture, scene);
}

boost::shared_ptr<LuaScene> LuaScene::ReadSceneFile(const char *filename,
    SceneInfo *info)
{
  boost::shared_ptr<LuaScene> o = boost::shared_ptr<LuaScene>(new LuaScene());
  lua_State *lua = o->lua;
  set_pcall_callback(add_file_and_line);

  fs::path p(filename);
  p = fs::system_complete(p);
  o->script_path = p.parent_path();

  int s;
  bool insupport = true;
  s = luaL_dofile(lua, "luascene.lua");
  if (!s) {
    fs::path cwd = fs::current_path();
    fs::current_path(o->script_path);
    insupport = false;
    s = luaL_dofile(lua, filename);
    fs::current_path(cwd);
  }

  if (s) {
    if (insupport) {
      std::cout << "Error was raised in luascene.lua\n";
    }
    std::cout << "*** Lua returned an error:     \n"
              << lua_tostring(lua, -1) << "\n\n";
    lua_pop(lua, 1);
    return boost::shared_ptr<LuaScene>();
  }
 

  SceneInfo si;
  try {
    si = object_cast<SceneInfo>(globals(lua)["scene_info"]);
  } catch (luabind::cast_failed) {
    std::cout << "(No scene_info defined, using defaults)\n";
  }

  if (info) *info = si;

  try {
    o->scene_function = globals(lua)["scene"];
    if (!o->scene_function.is_valid() || type(o->scene_function) != LUA_TFUNCTION) {
      std::cout << "*** Lua scene file must define a function 'scene'\n";
      return boost::shared_ptr<LuaScene>();
    }
  } catch (luabind::error) {
    const char *s = lua_tostring(lua, -1);
    std::cout << "*** Lua returned an exception:\n" << s << "\n";
  } catch (std::exception e) {
    std::cout << "*** Unhandled exception:\n" << e.what() << "\n";
  }

  return o;
}

boost::shared_ptr<Scene> LuaScene::SceneAt(double clock) {
  boost::shared_ptr<Scene> o;

  fs::path cwd = fs::current_path();
  fs::current_path(script_path);

  try {
    o = object_cast<boost::shared_ptr<Scene> >(scene_function(clock));
  } catch (luabind::error) {
    const char *s = lua_tostring(lua, -1);
    std::cout << "*** Lua returned an exception:\n" << s << "\n";
  } catch (std::exception e) {
    std::cout << "*** Unhandled exception:\n" << e.what() << "\n";
  }

  fs::current_path(cwd);

  return o;
}

// Create Lua bindings for all the scene objects and auxilliary classes
void LuaScene::bind_classes() {
  module(lua) [
    class_<Vector>("Vector")
      .def(constructor<double, double, double>())
      .def(-const_self)
      .def(const_self + Vector())
      .def(const_self - Vector())
      .def(const_self * double())
      .def(double() * const_self)
      .def_readwrite("x", &Vector::x)
      .def_readwrite("y", &Vector::y)
      .def_readwrite("z", &Vector::z),

    class_<Normal>("Normal")
      .def(constructor<double, double, double>()),

    class_<Point>("Point")
      .def(constructor<double, double, double>())
      .def(const_self + Vector())
      .def(const_self - Vector())
      .def(const_self - Point())
      .def(const_self * double())
      .def(double() * const_self)
      .def_readwrite("x", &Point::x)
      .def_readwrite("y", &Point::y)
      .def_readwrite("z", &Point::z),

    class_<Color>("Color")
      .def(constructor<double, double, double>()),

    class_<Scene, boost::shared_ptr<Scene> >("Scene")
      .def(constructor<>())
      .def("add_entity_", &Scene::AddEntity)
      .def("add_light", &Scene::AddLight)
      .def_readwrite("background_color", &Scene::background_color)
      .def_readwrite("camera", &Scene::camera),

    class_<Transform>("Transform")
      .def(const_self * Transform())
      .def("inverse", &Transform::Inverse),
    def("Translate", &Translate),
    def("Scale", &Scale),
    def("Rotate", &Rotate),
    def("LookAt", &LookAt),

    class_<Entity, boost::shared_ptr<Entity> >("Entity")
      .def_readwrite("texture", &Entity::texture),

    class_<SphereEntity, Entity, boost::shared_ptr<Entity> >("Sphere")
      .def(constructor<Transform const&>())
      .def(constructor<>()),

    class_<CylinderEntity, Entity, boost::shared_ptr<Entity> >("Cylinder")
      .def(constructor<Transform const&>())
      .def(constructor<>()),

    class_<ConeEntity, Entity, boost::shared_ptr<Entity> >("Cone")
      .def(constructor<Transform const&>())
      .def(constructor<>()),

    class_<MeshFace>("MeshFace")
      .def_readwrite("v1", &MeshFace::v1)
      .def_readwrite("v2", &MeshFace::v2)
      .def_readwrite("v3", &MeshFace::v3)
      .def_readwrite("n1", &MeshFace::n1)
      .def_readwrite("n2", &MeshFace::n2)
      .def_readwrite("n3", &MeshFace::n3)
      .def_readwrite("t1", &MeshFace::t1)
      .def_readwrite("t2", &MeshFace::t2)
      .def_readwrite("t3", &MeshFace::t3)
      .def(constructor<>()),
    class_<UV>("UV")
      .def_readwrite("u", &UV::u)
      .def_readwrite("v", &UV::v)
      .def(constructor<double, double>()),
    def("AddMesh", &lua_add_mesh),

    class_<Camera, boost::shared_ptr<Camera> >("Camera")
      .def_readwrite("transform", &Camera::transformation)
      .def_readwrite("up", &Camera::up)
      .def_readwrite("right", &Camera::right)
      .def_readwrite("direction", &Camera::direction),

    class_<Perspective, Camera, boost::shared_ptr<Camera> >("Perspective")
      .def(constructor<double, double>())
      .def(constructor<double>())
      .def(constructor<>()),

    class_<Light, boost::shared_ptr<Light> >("Light")
      .def(constructor<>())
      .def_readwrite("color", &Light::color)
      .def_readwrite("rows", &Light::rows)
      .def_readwrite("cols", &Light::cols)
      .def_readwrite("area", &Light::area)
      .def_readwrite("transform", &Light::transformation),

    class_<Texture, boost::shared_ptr<Texture> >("Texture")
      .def(constructor<>())
      .def_readwrite("finish", &Texture::finish)
      .def_readwrite("pigment", &Texture::pigment),

    class_<TextureMap2D, boost::shared_ptr<TextureMap2D> >("TextureMap2D"),
    class_<UVMap, TextureMap2D, boost::shared_ptr<TextureMap2D> >("UVMap")
      .def(constructor<>())
      .def_readwrite("scale_s", &UVMap::scale_s)
      .def_readwrite("scale_t", &UVMap::scale_t)
      .def_readwrite("translate_s", &UVMap::translate_s)
      .def_readwrite("translate_t", &UVMap::translate_t),

    class_<TextureMap3D, boost::shared_ptr<TextureMap2D> >("TextureMap3D"),
    class_<LinearMap, TextureMap3D, boost::shared_ptr<TextureMap3D> >("LinearMap")
      .def(constructor<>())
      .def_readwrite("transform", &LinearMap::transform),

    class_<Finish, boost::shared_ptr<Finish> >("Finish")
      .def(constructor<>())
      .def_readwrite("ambient", &Finish::ambient)
      .def_readwrite("diffuse", &Finish::diffuse)
      .def_readwrite("specular", &Finish::specular)
      .def_readwrite("exponent", &Finish::exponent)
      .def_readwrite("reflection", &Finish::reflection)
      .def_readwrite("refraction", &Finish::refraction)
      .def_readwrite("refraction_index", &Finish::refraction_index)
      .def_readwrite("absorption", &Finish::absorption),

    class_<Pigment, boost::shared_ptr<Pigment> >("Pigment"),

    class_<ColorPigment, Pigment, boost::shared_ptr<Pigment> >("ColorPigment")
      .def(constructor<>())
      .def(constructor<Color>())
      .def_readwrite("color", &ColorPigment::color),

    class_<CheckerPigment2D, Pigment, boost::shared_ptr<Pigment> >("CheckerPigment2D")
      .def(constructor<>())
      .def_readwrite("pigment1", &CheckerPigment2D::pigment1)
      .def_readwrite("pigment2", &CheckerPigment2D::pigment2)
      .def_readwrite("texture_map", &CheckerPigment2D::texture_map),

    class_<ImagePigment2D, Pigment, boost::shared_ptr<Pigment> >("ImagePigment2D")
      .def(constructor<std::string const&>())
      .def_readwrite("texture_map", &ImagePigment2D::texture_map),

    class_<CheckerPigment3D, Pigment, boost::shared_ptr<Pigment> >("CheckerPigment3D")
      .def(constructor<>())
      .def_readwrite("pigment1", &CheckerPigment3D::pigment1)
      .def_readwrite("pigment2", &CheckerPigment3D::pigment2)
      .def_readwrite("texture_map", &CheckerPigment3D::texture_map),

    class_<MarblePigment3D, Pigment, boost::shared_ptr<Pigment> >("MarblePigment3D")
      .def(constructor<>())
      .def_readwrite("color1", &MarblePigment3D::color1)
      .def_readwrite("color2", &MarblePigment3D::color2)
      .def_readwrite("power", &MarblePigment3D::power)
      .def_readwrite("size", &MarblePigment3D::size)
      .def_readwrite("frequency", &MarblePigment3D::frequency)
      .def_readwrite("texture_map", &MarblePigment3D::texture_map),

    class_<WoodPigment3D, Pigment, boost::shared_ptr<Pigment> >("WoodPigment3D")
      .def(constructor<>())
      .def_readwrite("color1", &WoodPigment3D::color1)
      .def_readwrite("color2", &WoodPigment3D::color2)
      .def_readwrite("power", &WoodPigment3D::power)
      .def_readwrite("size", &WoodPigment3D::size)
      .def_readwrite("frequency", &WoodPigment3D::frequency)
      .def_readwrite("texture_map", &WoodPigment3D::texture_map),

    class_<SceneInfo>("SceneInfo")
      .def(constructor<>())
      .def_readwrite("image_width", &SceneInfo::image_width)
      .def_readwrite("image_height", &SceneInfo::image_height)
      .def_readwrite("samples", &SceneInfo::samples)
      .def_readwrite("frames", &SceneInfo::frames)
      .def_readwrite("start_clock", &SceneInfo::start_clock)
      .def_readwrite("end_clock", &SceneInfo::end_clock)
  ];
}

}