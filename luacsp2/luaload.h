#ifndef LL_FILE_H_INCLUDED
#define LL_FILE_H_INCLUDED
#include "lua/lua.h"

const char *luaload_file_read (lua_State *L, void *ud, size_t *size);
int luaload_file(lua_State* s,const char* fn,const char* md);

#endif // LL_FILE_H_INCLUDED
