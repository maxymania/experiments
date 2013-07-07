#ifndef VMCHANNEL_H_INCLUDED
#define VMCHANNEL_H_INCLUDED
#include "vmcontext.h"

struct header_s {
    int type;
};

#define TCUSTOM 0x11000f0
#define TCHANNEL 0x220002d
#define TXCHANNEL 0x2200077
#define TIODESCR 0x2203311
#define TBYTEBUFFER 0x4123

struct custom {
    int type;
    void* ptr;
};
struct channel {
    int type;
    QUEUE tasks;
    int isProducer;
};
struct xchannel {
    int type;
    QUEUE data;
    int isProducer;
    mutex_id mutex;
};
struct bytebuffer {
    int type;
    size_t capacity;
    size_t limit;
    size_t pos;
    char data[0];
};

struct iodescr;
typedef int (*iodescr_read)(struct iodescr* iod,char* data,size_t size);
typedef void (*iodescr_write)(struct iodescr* iod,const char* data,size_t size);
struct iodescr {
    int type;
    mutex_id pending_mutex;
    QUEUE pending;
    void* data;
    iodescr_read read;
    iodescr_write write;
};

void vmchannel_install(lua_State* s);
int vmchannel_gc(lua_State* s);
int vmchannel_create(lua_State* s);
int vmchannel_buffer_create(lua_State* s);
int vmchannel_buffer_string(lua_State* s);
int vmchannel_buffer_put(lua_State* s);
int vmchannel_buffer_flip(lua_State* s);
int vmchannel_buffer_reset(lua_State* s);
int vmchannel_buffer_length(lua_State* s);
int vmchannel_lua_write(lua_State* s);
int vmchannel_lua_xfsend(lua_State* s);
int vmchannel_receive(struct channel* ch,TASKLET* t);
int vmchannel_send(struct channel* ch,TASKLET* t);

int vmchannel_xch_receive(struct xchannel* ch,TASKLET* t);
void vmchannel_xch_send(struct xchannel* ch,TASKLET* t);
void vmchannel_xch_sendval(struct xchannel* ch,struct xcvalue* value);
void vmchannel_xv_push(struct xcvalue* from,lua_State* to);
struct xcvalue* vmchannel_xv_pop(lua_State* from);
struct xcvalue* vmchannel_xv_string(const char* d,size_t size,int copy);
struct xcvalue* vmchannel_xv_userdata(void* ud);

struct xchannel* vmchannel_xchannel_create();
void vmchannel_xchannel_destroy(struct xchannel* ch);

int  vmchannel_io_read(struct iodescr* iod, TASKLET* t);
void vmchannel_io_readable(struct iodescr* iod);
void vmchannel_io_write(struct iodescr* iod, const char* data,size_t size);
void vmchannel_io_init(struct iodescr* iod);
void vmchannel_io_uninit(struct iodescr* iod);

#endif // VMCHANNEL_H_INCLUDED
