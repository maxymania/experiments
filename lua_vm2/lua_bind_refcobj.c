#include "lua_bind_refcobj.h"
#include "src/lauxlib.h"
//#include "src/lualib.h"

#define REFOBJ_OBJ "_RefcObject_"
#define REFOBJ_CACHE "_RefcO_Cache"

struct _Object_S{
	RefcObject* ref;
};
typedef struct _Object_S Object;

static void push_refc_special(lua_State *L,pthread_spinlock_t* lock,RefcObject** ref){
	Object* o;
	RefcObject* copy_of_ref;
	int top = lua_gettop(L);
	lua_getfield(L,LUA_REGISTRYINDEX,REFOBJ_CACHE);
	lua_rawgetp(L,top+1,*ref);
	if(lua_isnil(L,top+2)){
		lua_settop(L,top+1);
		o = lua_newuserdata(L,sizeof(Object));
		pthread_spin_lock(lock);
		copy_of_ref= *ref;
		RefcObject_grab(copy_of_ref);
		pthread_spin_unlock(lock);
		o->ref = copy_of_ref;
		luaL_getmetatable(L,REFOBJ_OBJ);
		lua_setmetatable(L,top+2);
		lua_pushvalue(L,top+2);
		lua_rawsetp(L,top+1,copy_of_ref);
	}
	lua_settop(L,top+2);
	lua_replace(L,top+1);
}

static int ref_gc(lua_State *L){
	Object* o = luaL_checkudata(L,1,REFOBJ_OBJ);
	RefcObject_drop(o->ref);
	return 0;
}


static int ref_get_num(lua_State *L){
	Object* o = luaL_checkudata(L,1,REFOBJ_OBJ);
	lua_Integer i = lua_tointeger(L,2);
	if((i<0)||(i>(o->ref->nnum))){
		lua_pushstring(L,"Index out of bounds");
		lua_error(L);
		return 0;
	}
	lua_pushnumber(L,o->ref->anum[i]);
	return 1;
}
static int ref_get_bool(lua_State *L){
	Object* o = luaL_checkudata(L,1,REFOBJ_OBJ);
	lua_Integer i = lua_tointeger(L,2);
	if((i<0)||(i>(o->ref->nbool))){
		lua_pushstring(L,"Index out of bounds");
		lua_error(L);
		return 0;
	}
	lua_pushboolean(L,o->ref->abool[i]);
	return 1;
}
static int ref_get_str(lua_State *L){
	Object* o = luaL_checkudata(L,1,REFOBJ_OBJ);
	lua_Integer i = lua_tointeger(L,2);
	if((i<0)||(i>(o->ref->nstr))){
		lua_pushstring(L,"Index out of bounds");
		lua_error(L);
		return 0;
	}
	pthread_spin_lock(&(o->ref->strlock));
	String str = o->ref->astr[i];
	lua_pushlstring(L,str.ptr,str.size);
	pthread_spin_unlock(&(o->ref->strlock));
	return 1;
}
static int ref_get_ref(lua_State *L){
	Object* o = luaL_checkudata(L,1,REFOBJ_OBJ);
	lua_Integer i = lua_tointeger(L,2);
	if((i<0)||(i>(o->ref->nref))){
		lua_pushstring(L,"Index out of bounds");
		lua_error(L);
		return 0;
	}
	push_refc_special(L,&(o->ref->rclock),&(o->ref->aref[i]));
	return 1;
}


static int ref_set_num(lua_State *L){
	Object* o = luaL_checkudata(L,1,REFOBJ_OBJ);
	lua_Integer i = lua_tointeger(L,2);
	if((i<0)||(i>(o->ref->nnum))){
		lua_pushstring(L,"Index out of bounds");
		lua_error(L);
		return 0;
	}
	o->ref->anum[i] = lua_tonumber(L,3);
	return 0;
}
static int ref_set_bool(lua_State *L){
	Object* o = luaL_checkudata(L,1,REFOBJ_OBJ);
	lua_Integer i = lua_tointeger(L,2);
	if((i<0)||(i>(o->ref->nbool))){
		lua_pushstring(L,"Index out of bounds");
		lua_error(L);
		return 0;
	}
	o->ref->abool[i] = lua_toboolean(L,3)?1:0;
	return 0;
}
static int ref_set_str(lua_State *L){
	Object* o = luaL_checkudata(L,1,REFOBJ_OBJ);
	lua_Integer i = lua_tointeger(L,2);
	if((i<0)||(i>(o->ref->nstr))){
		lua_pushstring(L,"Index out of bounds");
		lua_error(L);
		return 0;
	}
	pthread_spin_lock(&(o->ref->strlock));
	String str;
	str.ptr = lua_tolstring(L,3,&str.size);
	String_copy(&(o->ref->astr[i]),str);
	pthread_spin_unlock(&(o->ref->strlock));
	return 0;
}
static int ref_set_ref(lua_State *L){
	RefcObject* copy_of_ref;
	Object* o = luaL_checkudata(L,1,REFOBJ_OBJ);
	lua_Integer i = lua_tointeger(L,2);
	Object* o2 = luaL_testudata(L,3,REFOBJ_OBJ);
	if((i<0)||(i>(o->ref->nref))){
		lua_pushstring(L,"Index out of bounds");
		lua_error(L);
		return 0;
	}
	pthread_spin_lock(&(o->ref->rclock));
	copy_of_ref = o->ref->aref[i];
	o->ref->aref[i] = o2?(o2->ref):NULL;
	pthread_spin_unlock(&(o->ref->rclock));
	if(copy_of_ref)RefcObject_drop(copy_of_ref);
	return 0;
}

static int create_ref_metatable(lua_State *L){
	lua_newtable(L);
	lua_newtable(L);
	lua_pushstring(L,"v");
	lua_setfield(L,2,"__mode");
	lua_setmetatable (L,1);
	lua_setfield(L,LUA_REGISTRYINDEX,REFOBJ_CACHE);
	if(luaL_newmetatable(L,REFOBJ_OBJ)){
		lua_pushcfunction(L,ref_gc);
		lua_setfield(L,1,"__gc");
		lua_newtable(L);
		lua_setfield(L,1,"__metatable");
	}
	return 0;
}

static int ref_new_ref(lua_State *L){
	uint32_t
		nref = lua_tointeger(L,1),
		nnum = lua_tointeger(L,2),
		nbool = lua_tointeger(L,3),
		nstr = lua_tointeger(L,4);
	// --------
	RefcObject* ref=RefcObject_new(nref,nnum,nbool,nstr);
	push_refc(L,ref);
	return 1;
}

void init_refc(lua_State *L){
	lua_pushcfunction(L,create_ref_metatable);
	lua_call(L,0,0);
	lua_pushcfunction(L,ref_get_num);
	lua_setglobal(L,"getnum");
	lua_pushcfunction(L,ref_get_bool);
	lua_setglobal(L,"getbool");
	lua_pushcfunction(L,ref_get_str);
	lua_setglobal(L,"getstr");
	lua_pushcfunction(L,ref_get_ref);
	lua_setglobal(L,"getref");
	
	lua_pushcfunction(L,ref_set_num);
	lua_setglobal(L,"setnum");
	lua_pushcfunction(L,ref_set_bool);
	lua_setglobal(L,"setbool");
	lua_pushcfunction(L,ref_set_str);
	lua_setglobal(L,"setstr");
	lua_pushcfunction(L,ref_set_ref);
	lua_setglobal(L,"setref");
	lua_pushcfunction(L,ref_new_ref);
	lua_setglobal(L,"new");
}

void push_refc(lua_State *L,RefcObject* ref){
	Object* o;
	int top = lua_gettop(L);
	lua_getfield(L,LUA_REGISTRYINDEX,REFOBJ_CACHE);
	lua_rawgetp(L,top+1,ref);
	if(lua_isnil(L,top+2)){
		lua_settop(L,top+1);
		o = lua_newuserdata(L,sizeof(Object));
		o->ref = ref;
		RefcObject_grab(ref);
		luaL_getmetatable(L,REFOBJ_OBJ);
		lua_setmetatable(L,top+2);
		lua_pushvalue(L,top+2);
		lua_rawsetp(L,top+1,ref);
	}
	lua_settop(L,top+2);
	lua_replace(L,top+1);
}

