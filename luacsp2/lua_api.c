#include <stdint.h>
#include <stdio.h>
#include "lua_api.h"

void luaapi_install(lua_State* s){
    lua_register(s,"tostring",luaapi_tostring);
    lua_register(s,"type",luaapi_type);
}
int luaapi_tostring(lua_State* s){
    const char* ptr;
    char buf[20];
    size_t size;
    int tp;
    int num;
    lua_settop(s,1);
    tp=lua_type(s,1);
    switch(tp){
    case LUA_TNUMBER:
    case LUA_TSTRING:
        ptr=lua_tolstring(s,1,&size);
        lua_pushlstring(s,ptr,size);
        lua_remove(s,1);
        break;
    case LUA_TNIL:
        lua_pushstring(s,"nil");
        lua_remove(s,1);
        break;
    case LUA_TBOOLEAN:
        lua_pushstring(s,lua_toboolean(s,1)?"true":"false");
        lua_remove(s,1);
        break;
    default:
        num=(int)lua_topointer(s,1);
        lua_pushlstring(s,buf,snprintf(buf,20,"0x%x",num));
        lua_remove(s,1);
        break;
    }
    return 1;
}
int luaapi_type(lua_State* s){
    int tp;
    lua_settop(s,1);
    tp=lua_type(s,1);
    lua_settop(s,0);
    lua_pushstring(s,lua_typename(s,tp));
    return 1;
}

void luaapiA_install(lua_State* s){
    lua_register(s,"trap",luaapiA_trap);
    lua_register(s,"send",luaapiA_send);
    lua_register(s,"recv",luaapiA_recv);
    lua_register(s,"spawn",luaapiA_spawn);
    lua_register(s,"read",luaapiA_read);
}
int luaapiA_trap(lua_State* s){
    lua_settop(s,0);
    return lua_yield(s,0);
    //return lua_yield(s,lua_gettop(s));
}
int luaapiA_send(lua_State* s){
    lua_settop(s,2);
    return lua_yield(s,2);
}
int luaapiA_recv(lua_State* s){
    lua_settop(s,1);
    return lua_yield(s,1);
}
int luaapiA_spawn(lua_State* s){
    lua_settop(s,1);
    lua_pushstring(s,"s");
    lua_pushvalue(s,1);
    return lua_yield(s,2);
}
int luaapiA_read(lua_State* s){
    lua_settop(s,2);
    return lua_yield(s,2);
}
