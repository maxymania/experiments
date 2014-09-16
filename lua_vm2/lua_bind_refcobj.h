#ifndef LUABINDREFC
#define LUABINDREFC
#include "refcobj.h"
#include "src/lua.h"

void init_refc(lua_State *L);
void push_refc(lua_State *L,RefcObject* ref);

#endif

