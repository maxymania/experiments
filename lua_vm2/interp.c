#include <stdio.h>
#include "src/lua.h"
#include "src/lauxlib.h"
#include "lua_bind_refcobj.h"

int main(){
	lua_State* L = luaL_newstate();
	luaopen_base(L);
	lua_settop(L,0);
	init_refc(L);
	luaL_loadfile(L,"test.lua");
	lua_pcall(L,0,0,0);
	printf("OK!\n");
	lua_close(L);
}


