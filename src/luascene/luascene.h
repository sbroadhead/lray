#ifndef _RAYTRACE_LUASCENE_LUASCENE_H_
#define _RAYTRACE_LUASCENE_LUASCENE_H_

#include <boost/filesystem.hpp>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#include "common.h"
#include "renderer/scene.h"
#include "luabind/luabind.hpp"

namespace raytrace {;

class LuaScene {
public:
  ~LuaScene();

  boost::shared_ptr<Scene> SceneAt(double clock);
  static boost::shared_ptr<LuaScene> ReadSceneFile(const char *filename,
    SceneInfo *info);
private:
  LuaScene();

  luabind::object scene_function;
  void bind_classes();
  lua_State *lua;
  boost::filesystem::path script_path;
};

}

#endif