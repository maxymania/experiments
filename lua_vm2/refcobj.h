#ifndef REFOBJ_H
#define REFOBJ_H
#include <pthread.h>
#include "arc/arc.h"
#include "src/lua.h"
#include "copystr.h"

struct RefcObject_s;
typedef struct RefcObject_s RefcObject;
struct RefcObject_s{
	AtomicCount arco;
	pthread_spinlock_t rclock;
	pthread_spinlock_t strlock;
	uint32_t nref;
	RefcObject** aref;
	uint32_t nnum;
	lua_Number* anum;
	uint32_t nbool;
	uint8_t* abool;
	uint32_t nstr;
	String* astr;
};

RefcObject* RefcObject_new(uint32_t nref,uint32_t nnum,uint32_t nbool,uint32_t nstr);
// unsafe!!!(
void RefcObject_free(RefcObject* ref);
// )unsafe!!!

void RefcObject_grab(RefcObject* ref);
void RefcObject_drop(RefcObject* ref);


#endif

