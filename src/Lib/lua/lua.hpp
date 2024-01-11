// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

extern "C" {
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"
}

extern lua_State* globalLuaState;
