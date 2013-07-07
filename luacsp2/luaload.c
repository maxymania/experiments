#include <stdio.h>
#include "luaload.h"

struct loadfile_s {
    FILE* f;
    char data[1024];
};

const char *luaload_file_read (lua_State *L, void *ud, size_t *size){
    struct loadfile_s *f = (struct loadfile_s*)ud;
    (void)L;  /* not used */
    if(feof(f->f)){
        fclose(f->f);
        return NULL;
    }else{
        *size=fread(f->data,1,1024,f->f);
        return f->data;
    }
}
int luaload_file(lua_State* s,const char* fn,const char* md){
    struct loadfile_s fs;
    fs.f=fopen(fn,"rb");
    return lua_load(s,luaload_file_read,&fs,fn,md);
}
