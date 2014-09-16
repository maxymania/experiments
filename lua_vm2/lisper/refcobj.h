#ifndef REFOBJ_H
#define REFOBJ_H
#include <pthread.h>
#include "arc/arc.h"

enum RefcObjTypes_e {
	RCOT_INT,RCOT_STR,RCOT_LIST,RCOT_VECTOR,
};
typedef enum RefcObjTypes_e RefcObjTypes;

struct RefcObject_s;
typedef struct RefcObject_s RefcObject;
typedef RefcObject* RefcObjP;
struct RefcObject_s{
	AtomicCount arco;
	RefcObjTypes type;
	union{
		int64_t i;
		struct {
			uint32_t len;
			char* array;
		} str;
		struct {
			pthread_spinlock_t rclock;
			RefcObjP car,cdr;
		} list;
		struct {
			pthread_spinlock_t rclock;
			uint32_t len;
			RefcObjP* array;
		} vector;
	};
};

RefcObject* RefcObject_new(RefcObjTypes type,uint32_t num);
// unsafe!!!(
void RefcObject_free(RefcObject* ref);
// )unsafe!!!

void RefcObject_grab(RefcObject* ref);
void RefcObject_drop(RefcObject* ref);


#endif

