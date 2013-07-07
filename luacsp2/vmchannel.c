#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vmchannel.h"
#include "lua_api.h"

void vmchannel_install(lua_State* s){
    lua_createtable(s,0,2);
    lua_pushcfunction(s,vmchannel_gc);
    lua_setfield(s,-2,"__gc");
    lua_createtable(s,0,2);
    lua_pushcfunction(s,luaapiA_send);
    lua_setfield(s,-2,"send");
    lua_pushcfunction(s,luaapiA_recv);
    lua_setfield(s,-2,"recv");
    lua_setfield(s,-2,"__index");
    lua_setglobal(s,"__mt_ch__");
    lua_register(s,"chan",vmchannel_create);
    //
    lua_createtable(s,0,2);
    lua_pushcfunction(s,vmchannel_buffer_length);
    lua_setfield(s,-2,"__len");
    lua_createtable(s,0,6);
    lua_pushcfunction(s,vmchannel_buffer_string);
    lua_setfield(s,-2,"string");
    lua_pushcfunction(s,vmchannel_buffer_put);
    lua_setfield(s,-2,"put");
    lua_pushcfunction(s,vmchannel_buffer_flip);
    lua_setfield(s,-2,"flip");
    lua_pushcfunction(s,vmchannel_buffer_reset);
    lua_setfield(s,-2,"reset");
    lua_pushcfunction(s,vmchannel_buffer_length);
    lua_setfield(s,-2,"length");
    lua_setfield(s,-2,"__index");
    lua_setglobal(s,"__mt_buf__");
    lua_register(s,"buffer",vmchannel_buffer_create);
    //
    lua_register(s,"write",vmchannel_lua_write);
    lua_register(s,"_xfsend",vmchannel_lua_xfsend);
    //
}

int vmchannel_gc(lua_State* s){
    struct channel* ch;
    ch=lua_touserdata(s,1);
    queue_destroy(&(ch->tasks));
    lua_pop(s,1);
    return 0;
}
int vmchannel_create(lua_State* s){
    lua_settop(s,0);
    struct channel* ch;
    ch = lua_newuserdata(s,sizeof(struct channel));
    ch->type=TCHANNEL;
    queue_init(&(ch->tasks));
    ch->isProducer=0;
    lua_getglobal(s,"__mt_ch__");
    lua_setmetatable(s,1);
    return 1;
}

int vmchannel_buffer_create(lua_State* s){
    struct bytebuffer* buf;
    int n=lua_gettop(s);
    size_t size;
    if(n==0)return 0;
    if(!lua_isnumber(s,1))return 0;
    size = lua_tointeger(s,1);
    lua_settop(s,0);
    buf = lua_newuserdata(s,sizeof(struct bytebuffer)+size);
    buf->type = TBYTEBUFFER;
    buf->capacity=size;
    buf->limit=size;
    buf->pos=0;
    lua_getglobal(s,"__mt_buf__");
    lua_setmetatable(s,1);
    return 1;
}
int vmchannel_buffer_string(lua_State* s){
    struct bytebuffer* buf;
    buf = lua_touserdata(s,1);
    if(!(buf && ((buf->type)==TBYTEBUFFER)))return 0;
    lua_pushlstring(s,&(buf->data[buf->pos]),(buf->limit)-(buf->pos));
    return 1;
}
int vmchannel_buffer_put(lua_State* s){
    struct bytebuffer* buf;
    size_t size1;
    size_t size2;
    const char* ptr;
    lua_settop(s,2);
    buf = lua_touserdata(s,1);
    if(!(buf && ((buf->type)==TBYTEBUFFER)))return 0;
    size1=(buf->limit)-(buf->pos);
    if(lua_isnumber(s,2)){
        if(size1>0){
            buf->data[buf->pos]=(char)(lua_tointeger(s,2));
            buf->pos++;
        }
    }else if(lua_isstring(s,2)){
        ptr = lua_tolstring(s,2,&size2);
        if(size1>size2)size1=size2;
        memcpy(&(buf->data[buf->pos]),ptr,size1);
        buf->pos+=size1;
    }
    lua_settop(s,1);
    return 1;
}
int vmchannel_buffer_flip(lua_State* s){
    struct bytebuffer* buf;
    if(!lua_gettop(s))return 0;
    buf = lua_touserdata(s,1);
    if(!(buf && ((buf->type)==TBYTEBUFFER)))return 0;
    buf->limit=buf->pos;
    buf->pos=0;
    return 1;
}
int vmchannel_buffer_reset(lua_State* s){
    struct bytebuffer* buf;
    if(!lua_gettop(s))return 0;
    buf = lua_touserdata(s,1);
    if(!(buf && ((buf->type)==TBYTEBUFFER)))return 0;
    buf->limit=buf->capacity;
    buf->pos=0;
    return 1;
}
int vmchannel_buffer_length(lua_State* s){
    struct bytebuffer* buf;
    if(!lua_gettop(s))return 0;
    buf = lua_touserdata(s,1);
    if(!(buf && ((buf->type)==TBYTEBUFFER)))return 0;
    lua_pushinteger(s,(buf->limit)-(buf->pos));
    lua_remove(s,1);
    return 1;
}
int vmchannel_lua_write(lua_State* s){
    struct iodescr* iod;
    struct bytebuffer* buf;
    size_t size;
    const char* ptr;
    iod=lua_touserdata(s,1);
    if(iod && ((iod->type)==TIODESCR)){
        buf=lua_touserdata(s,2);
        if(buf && ((buf->type)==TBYTEBUFFER)){
            vmchannel_io_write(iod,&(buf->data[buf->pos]),(buf->limit)-(buf->pos));
            buf->pos=buf->limit;
        }else if(lua_isstring(s,2)||lua_isnumber(s,2)){ // strings and numbers can be converted to strings by lua
            ptr = lua_tolstring(s,2,&size);
            vmchannel_io_write(iod,ptr,size);
        }
    }
    return 0;
}
int vmchannel_lua_xfsend(lua_State* s){
    struct xchannel* ch;
    struct xcvalue* xv;
    if(lua_gettop(s)<2)return 0;
    ch = lua_touserdata(s,1);
    if(!(ch && ((ch->type)==TXCHANNEL)))return 0;
    xv=vmchannel_xv_pop(s);
    vmchannel_xch_sendval(ch,xv);
    return 0;
}

int vmchannel_receive(struct channel* ch,TASKLET* t){
    TASKLET* other;
    if(ch->isProducer){
        other = queue_pop(&(ch->tasks));
        if(other){
            other->nargs--;
            lua_pop(t->state,1);
            lua_xmove(other->state,t->state,1);
            context_task_push(other);
            return 1;
        }
        ch->isProducer=0;
    }
    queue_push(&(ch->tasks),t);
    return 0;
}
int vmchannel_send(struct channel* ch,TASKLET* t){
    TASKLET* other;
    if(!(ch->isProducer)){
        other = queue_pop(&(ch->tasks));
        if(other){
            t->nargs--;
            lua_pop(other->state,1);
            lua_xmove(t->state,other->state,1);
            context_task_push(other);
            return 1;
        }
        ch->isProducer=1;
    }
    queue_push(&(ch->tasks),t);
    return 0;
}

int vmchannel_xch_receive(struct xchannel* ch,TASKLET* t){
    struct xcvalue* value;
    mutex_lock(ch->mutex);
    if(ch->isProducer){
        value = queue_pop(&(ch->data));
        if(value){
            lua_pop(t->state,1);
            vmchannel_xv_push(value,t->state);
            mutex_unlock(ch->mutex);
            return 1;
        }
        ch->isProducer=0;
    }
    queue_push(&(ch->data),t);
    mutex_unlock(ch->mutex);
    return 0;
}
/*
void vmchannel_xch_send(struct xchannel* ch,TASKLET* t){
    TASKLET* other;
    struct xcvalue* value;
    t->nargs--;
    value = vmchannel_xv_pop(t->state);
    mutex_lock(ch->mutex);
    if(!(ch->isProducer)){
        other = queue_pop(&(ch->data));
        if(other){
            other->value = value;
            context_task_push(other);
            mutex_unlock(ch->mutex);
            return;
        }
        ch->isProducer=1;
    }
    queue_push(&(ch->data),value);
    mutex_unlock(ch->mutex);
    return;
}
// */
void vmchannel_xch_send(struct xchannel* ch,TASKLET* t){
    struct xcvalue* value;
    t->nargs--;
    value = vmchannel_xv_pop(t->state);
    vmchannel_xch_sendval(ch,value);
}

void vmchannel_xch_sendval(struct xchannel* ch,struct xcvalue* value){
    TASKLET* other;
    mutex_lock(ch->mutex);
    if(!(ch->isProducer)){
        other = queue_pop(&(ch->data));
        if(other){
            other->value = value;
            context_task_push(other);
            mutex_unlock(ch->mutex);
            return;
        }
        ch->isProducer=1;
    }
    queue_push(&(ch->data),value);
    mutex_unlock(ch->mutex);
    return;
}

void vmchannel_xv_push(struct xcvalue* from,lua_State* to){
    switch(from->tp){
    case LUA_TNIL:
        lua_pushnil(to);
        break;
    case LUA_TNUMBER:
        lua_pushnumber(to,from->number);
        break;
    case LUA_TBOOLEAN:
        lua_pushboolean(to,from->boolean);
        break;
    case LUA_TLIGHTUSERDATA:
        lua_pushlightuserdata(to,from->light_ud);
        break;
    case LUA_TSTRING:
        lua_pushlstring(to,from->str,from->strsize);
        free((void*)(from->str));
        break;
    }
    free(from);
}
struct xcvalue* vmchannel_xv_pop(lua_State* from){
    struct xcvalue* ret=malloc(sizeof(struct xcvalue));
    const char* ptr;
    size_t size;
    int tp;
    tp = lua_type(from,-1);
    ret->tp=tp;
    switch(tp){
    case LUA_TNUMBER:
        ret->tp=LUA_TNUMBER;
        ret->number=lua_tonumber(from,-1);
        break;
    case LUA_TSTRING:
        ret->tp=LUA_TSTRING;
        ptr = lua_tolstring(from,-1,&size);
        ret->str=malloc(size);
        ret->strsize=size;
        memcpy((void*)ret->str,(const void*)ptr,size);
        break;
    case LUA_TBOOLEAN:
        ret->tp=LUA_TBOOLEAN;
        ret->boolean = lua_toboolean(from,-1);
        break;
    case LUA_TLIGHTUSERDATA:
        ret->tp=LUA_TLIGHTUSERDATA;
        ret->light_ud=lua_touserdata(from,-1);
        break;
    default:// this also includes nil
        ret->tp=LUA_TNIL;
        break;
    }
    lua_pop(from,1);
    return ret;
}

struct xcvalue* vmchannel_xv_string(const char* d,size_t size,int copy){
    struct xcvalue* value=malloc(sizeof(struct xcvalue));
    char* ptr;
    value->tp=LUA_TSTRING;
    value->strsize=size;
    if(copy){
        ptr=malloc(size);
        memcpy((void*)ptr,(const void*)d,size);
        value->str=ptr;
    }else
        value->str=d;
    return value;
}
struct xcvalue* vmchannel_xv_userdata(void* ud){
    struct xcvalue* value=malloc(sizeof(struct xcvalue));
    value->tp=LUA_TLIGHTUSERDATA;
    value->light_ud=ud;
    return value;
}

struct xchannel* vmchannel_xchannel_create(){
    struct xchannel* ch;
    ch = malloc(sizeof(struct xchannel));
    ch->type=TXCHANNEL;
    queue_init(&(ch->data));
    ch->isProducer=0;
    ch->mutex=mutex_create();
    return ch;
}
void vmchannel_xchannel_destroy(struct xchannel* ch){
    queue_destroy(&(ch->data));
    mutex_delete(ch->mutex);
    // TODO: consider to destroy the xcvalue objects
    free(ch);
}
int vmchannel_io_read(struct iodescr* iod, TASKLET* t){
    struct bytebuffer *buf;
    int res;
    buf = lua_touserdata(t->state,-1);
    if(!buf)return 1;
    if(buf->type!=TBYTEBUFFER)return 1;
    if((buf->pos)>=(buf->limit))return 1;
    //t->curbuf=buf;
    res = iod->read(iod,&(buf->data[buf->pos]),(buf->pos)-(buf->limit));
    if(res<0)return 1;
    if(res>0){
        buf->pos+=res;
        return 1;
    }
    t->curbuf=buf;
    mutex_lock(iod->pending_mutex);
        queue_push(&(iod->pending),t);
    mutex_unlock(iod->pending_mutex);
    return 0;
}
void vmchannel_io_readable(struct iodescr* iod){
    TASKLET* t;
    struct bytebuffer *buf;
    int res;
    mutex_lock(iod->pending_mutex);
    for(;;){
        t = queue_peek(&(iod->pending));
        if(!t)return;
        buf=t->curbuf;
        res = iod->read(iod,&(buf->data[buf->pos]),(buf->pos)-(buf->limit));
        if(res<0){
            //continue;
        }else if(res>0){
            buf->pos+=res;
            //continue;
        }else{
            break;
        }
        queue_pop(&(iod->pending));
        t->curbuf=NULL;
        context_task_push(t);
    }
    mutex_unlock(iod->pending_mutex);
}
void vmchannel_io_write(struct iodescr* iod, const char* data,size_t size){
    iod->write(iod,data,size);
}
void vmchannel_io_init(struct iodescr* iod){
    iod->type = TIODESCR;
    iod->pending_mutex = mutex_create();
    queue_init(&(iod->pending));
}
void vmchannel_io_uninit(struct iodescr* iod){
    mutex_delete(iod->pending_mutex);
    queue_destroy(&(iod->pending));
}

