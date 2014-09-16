#include <stdlib.h>
#include <string.h>
#include "refcobj.h"

static void* alloc_z(size_t s){
	void* r = malloc(s);
	memset(r,0,s);
	return r;
}
#define malloc alloc_z

RefcObject* RefcObject_new(RefcObjTypes type,uint32_t num){
	uint32_t i,n;
	RefcObject* ref;
	switch(type){
	case RCOT_STR:
		ref = malloc(sizeof(RefcObject)+num+1);
		ref->str.len = num;
		ref->str.array = ((void*)ref)+sizeof(RefcObject);
		break;
	case RCOT_LIST:
		ref = malloc(sizeof(RefcObject));
		pthread_spin_init(&(ref->list.rclock),PTHREAD_PROCESS_PRIVATE);
		ref->list.car = NULL;
		ref->list.cdr = NULL;
		break;
	case RCOT_VECTOR:
		ref = malloc(sizeof(RefcObject)+(sizeof(RefcObjP)*num));
		pthread_spin_init(&(ref->vector.rclock),PTHREAD_PROCESS_PRIVATE);
		ref->vector.len = num;
		ref->vector.array = ((void*)ref)+sizeof(RefcObject);
		break;
	default:
		ref = malloc(sizeof(RefcObject));
	}
	AtomicCount_init(&(ref->arco));
	ref->type = type;
	RefcObject_grab(ref);
	return ref;
}
void RefcObject_free(RefcObject* ref){
	uint32_t i,n;
	AtomicCount_destroy(&(ref->arco));
	switch(ref->type){
	case RCOT_LIST:
		pthread_spin_destroy(&(ref->list.rclock));
		RefcObject_drop(ref->list.car);
		RefcObject_drop(ref->list.cdr);
		break;
	case RCOT_VECTOR:
		pthread_spin_destroy(&(ref->vector.rclock));
		for(i=0,n=ref->vector.len;i<n;++i)
			RefcObject_drop(ref->vector.array[i]);
		break;
	}
	free(ref);
}
void RefcObject_grab(RefcObject* ref){
	if(!ref)return;
	AtomicCount_incr(&(ref->arco));
}
void RefcObject_drop(RefcObject* ref){
	if(!ref)return;
	if(!AtomicCount_decr(&(ref->arco)))
		RefcObject_free(ref);
}


