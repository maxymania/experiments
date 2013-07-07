#ifndef LUA_API_H_INCLUDED
#define LUA_API_H_INCLUDED
#include "lua/lua.h"

void luaapi_install(lua_State* s);
int luaapi_tostring(lua_State* s);
int luaapi_type(lua_State* s);

void luaapiA_install(lua_State* s);
int luaapiA_trap(lua_State* s);
int luaapiA_send(lua_State* s);
int luaapiA_recv(lua_State* s);
int luaapiA_spawn(lua_State* s);
int luaapiA_read(lua_State* s);

#endif // LUA_API_H_INCLUDED
