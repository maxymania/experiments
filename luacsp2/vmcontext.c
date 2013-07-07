#include <stdio.h>
#include <stdlib.h>
#include "vmcontext.h"
#include "vmchannel.h"
#include "lua_api.h"

#define THREADS_TABLE "__threads__"

void* context_alloc(void *ud,void *ptr,size_t osize,size_t nsize){
    (void)ud;
    if(osize){
        if(nsize)
            return realloc(ptr,nsize);
        else
            free(ptr);
        return NULL;
    }else{
        if(nsize)
            return malloc(nsize);
        return NULL;
    }
    return NULL;
}

CONTEXT* context_new(){
    CONTEXT* c = malloc(sizeof(CONTEXT));
    queue_init(&(c->tasks));
    c->tasks_m=mutex_create();
    c->root=lua_newstate(context_alloc,c);
    lua_createtable(c->root,100,0);
    lua_setglobal(c->root,THREADS_TABLE);
    luaapi_install(c->root);
    luaapiA_install(c->root);
    vmchannel_install(c->root);
    c->id=1;
    return c;
}
void context_destroy(CONTEXT* c){
    queue_destroy(&(c->tasks));
    mutex_delete(c->tasks_m);
    lua_close(c->root);
    free(c);
}
void context_task_push(TASKLET* t){
    CONTEXT* c;
    c = t->context;
    mutex_lock(c->tasks_m);
        queue_push(&(c->tasks),t);
    mutex_unlock(c->tasks_m);
}
TASKLET* context_task_pop(CONTEXT* c){
    TASKLET* t;
    mutex_lock(c->tasks_m);
        t = queue_pop(&(c->tasks));
    mutex_unlock(c->tasks_m);
    return t;
}
TASKLET* context_spawn_internal(CONTEXT* c){
    TASKLET* t;
    int isnil;
    t = malloc(sizeof(TASKLET));
    t->context=c;
    lua_getglobal(c->root,THREADS_TABLE); // [__threads__]
test_pos:
    lua_rawgeti(c->root,-1,c->id); // [__threads__,id]
    isnil=lua_isnil(c->root,-1);
    lua_pop(c->root,1); // [__threads__]
    if(!isnil){ c->id++; goto test_pos; }
    t->id=c->id++;
    t->nargs=0;
    t->value=NULL;

    lua_pushinteger(c->root,t->id); // [__threads__,id]
    t->state = lua_newthread(c->root);  // [__threads__,id,thread]
    lua_settable(c->root,-3); // [__threads__]
    lua_pop(c->root,1); // []
    return t;
}
TASKLET* context_spawn(CONTEXT* c){
    TASKLET* t;
    t = context_spawn_internal(c);
    lua_xmove(c->root,t->state,1);
    return t;
}
TASKLET* context_spawnfrom(TASKLET* t){
    TASKLET* t2;
    t2 = context_spawn_internal(t->context);
    lua_xmove(t->state,t2->state,1);
    return t2;
}
int context_step(CONTEXT* c){
    int result;
    int num;
    TASKLET* t;
    TASKLET* t2;
    struct header_s* ud;
    const char* str;
    t=context_task_pop(c);
    if(!t)return 0;
    if(t->value){
        vmchannel_xv_push(t->value,t->state);
        t->value=NULL;
    }
    result = lua_resume(t->state,c->root,t->nargs);
    if(result==LUA_YIELD){
        num=lua_gettop(t->state);
        t->nargs=num;
        if(num>0){
            ud=lua_touserdata(t->state,1);
            if(ud){
                if(ud->type==TCHANNEL){
                    if(num>=2){
                        t->nargs=2;
                        lua_settop(t->state,2);
                        if(!vmchannel_send((struct channel*)ud,t))return 1;
                    }else{
                        if(!vmchannel_receive((struct channel*)ud,t))return 1;
                    }
                }else if(ud->type==TXCHANNEL){
                    if(num>=2){
                        t->nargs=2;
                        lua_settop(t->state,2);
                        vmchannel_xch_send((struct xchannel*)ud,t);
                    }else{
                        if(!vmchannel_xch_receive((struct xchannel*)ud,t))return 1;
                    }
                }else if(ud->type==TIODESCR){
                    if(num>=2){
                        if(!vmchannel_io_read((struct iodescr*)ud,t))return 1;
                    }
                }
            }else if(lua_isstring(t->state,1)){
                str=lua_tostring(t->state,1);
                switch(*str){
                case 's':
                    t2=context_spawnfrom(t);
                    context_task_push(t2);
                    break;
                }
            }
        }
        context_task_push(t);
        //printf("%d\n",lua_gettop(t->state));
    }else{
        if(lua_isstring(t->state,-1)){
            printf("exit: %s\n",lua_tostring(t->state,-1));
        }
        lua_getglobal(c->root,THREADS_TABLE);
        lua_pushnil(c->root);
        lua_rawseti(c->root,-2,t->id);
        if (c->id>t->id)
            c->id=t->id;
        lua_pop(c->root,1);
        free(t);
    }
    return 1;
}

