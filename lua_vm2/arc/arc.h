#ifndef ARC_ARC_H
#define ARC_ARC_H
#include <pthread.h>
#include <stdint.h>

struct AtomicCount_s{
	pthread_spinlock_t lock;
	uint64_t count;
};
typedef struct AtomicCount_s AtomicCount;

void AtomicCount_init(AtomicCount* count);
void AtomicCount_destroy(AtomicCount* count);
int AtomicCount_incr(AtomicCount* count);
int AtomicCount_decr(AtomicCount* count);

#endif

