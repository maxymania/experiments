#include <stdlib.h>
#include <string.h>
#include "refcobj.h"

static void* alloc_z(size_t s){
	void* r = malloc(s);
	memset(r,0,s);
	return r;
}
#define malloc alloc_z

RefcObject* RefcObject_new(uint32_t nref,uint32_t nnum,uint32_t nbool,uint32_t nstr){
	uint32_t i,n;
	RefcObject* ref = malloc(sizeof(RefcObject));
	AtomicCount_init(&(ref->arco));
	pthread_spin_init(&(ref->rclock),PTHREAD_PROCESS_PRIVATE);
	pthread_spin_init(&(ref->strlock),PTHREAD_PROCESS_PRIVATE);
	if(ref->nref = nref)
		ref->aref = malloc(nref*sizeof(RefcObject*));
	else ref->aref = NULL;
	if(ref->nnum = nnum)
		ref->anum = malloc(nref*sizeof(lua_Number));
	else ref->anum = NULL;
	if(ref->nbool = nbool)
		ref->abool = malloc(nref*sizeof(uint8_t));
	else ref->abool = NULL;
	if(ref->nstr = nstr)
		ref->astr = malloc(nref*sizeof(String));
	else ref->astr = NULL;
	for(i=0,n = ref->nnum; i<n;++i) ref->anum[i] = 0;
	return ref;
}
void RefcObject_free(RefcObject* ref){
	uint32_t i,n;
	AtomicCount_destroy(&(ref->arco));
	pthread_spin_destroy(&(ref->rclock));
	pthread_spin_destroy(&(ref->strlock));
	for(i=0,n = ref->nref; i<n;++i)
		RefcObject_drop(ref->aref[i]);
	for(i=0,n = ref->nstr; i<n;++i)
		String_drop(&(ref->astr[i]));
	if(ref->aref)free(ref->aref);
	if(ref->anum)free(ref->anum);
	if(ref->abool)free(ref->abool);
	if(ref->astr)free(ref->astr);
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


