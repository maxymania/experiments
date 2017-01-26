/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
#include <pthread.h>
#include <stdlib.h>
#include "../threadlayer.h"

typedef struct{
	CLE_SERVICE_ROUTINE routine;
	void*               sock;
	pthread_t           thread_handle;
} ThreadHandle;

static void* ThreadLayerThreadMainFunction(void* th){
	ThreadHandle* tth = th;
	tth->routine(tth->sock);
	return 0;
}

void* ThreadLayerCreateNewThread(CLE_SERVICE_ROUTINE routine,void* sock){
	ThreadHandle* handle = calloc(sizeof(ThreadHandle),1);
	if(!handle)return 0;
	handle->routine = routine;
	handle->sock = sock;
	if(pthread_create(&(handle->thread_handle), NULL, ThreadLayerThreadMainFunction, handle)){
		free(handle);
		return 0;
	}
	return handle;
}

