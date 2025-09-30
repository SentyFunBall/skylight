#pragma once
// A handful of functions for Lua scripts to use to test C++ <-> Lua interop.

#include <lua.hpp>

extern "C" {

namespace Lfuncs {
// require
int require(lua_State *L);

// length of a table
int tlength(lua_State *L);
} // namespace Lfuncs

namespace Skylight {
class Lua {
  public:
    struct LFuncs {
        static int gettime(lua_State* L);
        static int getConfigProperty(lua_State* L);
        static int setConfigProperty(lua_State* L);
    };
};
} // namespace Skylight

} // extern "C"