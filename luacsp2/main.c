#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "vmcontext.h"
#include "vmchannel.h"
#include "luaload.h"

struct mydata {
    const char* ptr;
    size_t size;
};

int my_print(lua_State* s){
    size_t siz;
    const char* data;
    int i,n;
    n = lua_gettop(s);
    for(i=1;i<=n;++i){
        data = lua_tolstring(s,i,&siz);
        fwrite(data,1,siz,stdout);
    }
    printf("\n");
    lua_pop(s,n);
    return 0;
}

int mydata_read(struct iodescr* iod,char* data,size_t size){
    struct mydata* obj=iod->data;
    int res = size;
    if(size>obj->size)res = obj->size;
    obj->size -= res;
    memcpy(data,obj->ptr,res);
    obj->ptr += res;
    return res;
}
void mydata_write(struct iodescr* iod,const char* data,size_t size){
    fwrite(data,size,1,stdout);
}

int main()
{
    struct iodescr* iod = malloc(sizeof(struct iodescr));
    struct mydata* md = malloc(sizeof(struct mydata));
    md->ptr = "Hallo";
    md->size= 5;
    vmchannel_io_init(iod);
    iod->data  = md;
    iod->read  = mydata_read;
    iod->write = mydata_write;
    int i=0;
    TASKLET* t;
    CONTEXT* ctx = context_new();
    lua_register(ctx->root,"print",my_print);
    lua_pushlightuserdata(ctx->root,vmchannel_xchannel_create());
    lua_setglobal(ctx->root,"specialChannel");
    lua_pushlightuserdata(ctx->root,iod);
    lua_setglobal(ctx->root,"inpt1");
    luaload_file(ctx->root,"K:\\data\\cpp\\lua-test\\csp2\\test1.lua","t");
    t = context_spawn(ctx);
    context_task_push(t);
    for(i=0;i<10;++i)
        context_step(ctx);
    md->ptr = "Welt";
    md->size= 4;
    vmchannel_io_readable(iod);
    for(i=0;i<10;++i)
        context_step(ctx);
    md->ptr = "!<ausrufezeichen>";
    md->size= 17;
    vmchannel_io_readable(iod);
    for(i=0;i<10;++i)
        context_step(ctx);
    context_destroy(ctx);
    //lua_load(s,readmore,fopen("K:\\data\\cpp\\lua-test\\csp2\\test1.lua","rb"),"test1.lua","t");
    //printf("Hello world!\n");
    return 0;
}

