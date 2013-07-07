#ifndef VMCONTEXT_H_INCLUDED
#define VMCONTEXT_H_INCLUDED
#include "mutex_api.h"
#include "queue.h"
#include "lua/lua.h"

struct xcvalue {
    int tp;
    union {
        lua_Number number;
        int boolean;
        void* light_ud;
        struct{
            const char* str;
            size_t strsize;
        };
    };
};
struct context {
    lua_State* root;
    QUEUE tasks;
    mutex_id tasks_m;
    int id;
};
struct tasklet {
    struct context* context;
    lua_State* state;
    int id;
    int nargs;
    struct xcvalue* value;
    struct bytebuffer* curbuf;
};
typedef struct context CONTEXT;
typedef struct tasklet TASKLET;

void* context_alloc(void *ud,void *ptr,size_t osize,size_t nsize);

CONTEXT* context_new();
void context_destroy(CONTEXT* c);
void context_task_push(TASKLET* t);
TASKLET* context_task_pop(CONTEXT* c);
TASKLET* context_spawn_internal(CONTEXT* c);
TASKLET* context_spawn(CONTEXT* c);
TASKLET* context_spawnfrom(TASKLET* t);
int context_step(CONTEXT* c);

#endif // VMCONTEXT_H_INCLUDED
