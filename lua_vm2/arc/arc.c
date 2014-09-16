#include "arc.h"

void AtomicCount_init(AtomicCount* count){
	pthread_spin_init(&(count->lock),PTHREAD_PROCESS_PRIVATE);
	count->count=0;
}
void AtomicCount_destroy(AtomicCount* count){
	pthread_spin_destroy(&(count->lock));
	count->count=0;
}
int AtomicCount_incr(AtomicCount* count){
	uint64_t r;
	pthread_spin_lock(&(count->lock));
	count->count++;
	r = count->count;
	pthread_spin_unlock(&(count->lock));
	return r!=0;
}
int AtomicCount_decr(AtomicCount* count){
	uint64_t r;
	pthread_spin_lock(&(count->lock));
	count->count--;
	r = count->count;
	pthread_spin_unlock(&(count->lock));
	return r!=0;
}

