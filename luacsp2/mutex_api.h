#ifndef MUTEX_API_H_INCLUDED
#define MUTEX_API_H_INCLUDED

typedef void* mutex_id;
mutex_id mutex_create();
void mutex_delete(mutex_id id);
void mutex_lock(mutex_id id);
void mutex_unlock(mutex_id id);

#endif // MUTEX_API_H_INCLUDED
