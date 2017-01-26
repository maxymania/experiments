/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
#include <stdlib.h>
#include <stdio.h>

#include <COM/Cle64.h>
#include "svc.h"
#include "threadlayer.h"
#include "svcmap.h"

#define RETURN_FALSE_UNLESS(x) if(!(x)) return 0

int CleCreateServiceLowLevel(void* ctx,CLE_SERVICE_ROUTINE dispatcher,const char* name) {
	(void)ctx;
	CLE_CONST_PTR key;
	void* thread;
	unsigned key_len;
	
	void** service_ptr = InternalContextAndNameCreateDispatcherSlot(ctx,name);
	
	printf("service_ptr = %p\n",service_ptr);
	
	SERVICE* service = calloc(sizeof(SERVICE),1);
	RETURN_FALSE_UNLESS(service);
	
	printf("service = %p\n",service);
	
	void *server = CleCreateServer();
	if(!server){
		free(service);
		return 0;
	}
	service->sock = server;
	
	printf("server = %p\n",server);
	
	if(!CleGenerateServiceKeyLocal(&key,&key_len)){
		free(service);
		CleCloseServer(server);
		return 0;
	}
	service->key     = key;
	service->key_len = key_len;
	
	printf("key, key_len = %p, %u\n",key, key_len);
	
	if(!CleBindServer(server,key,key_len)){
		free(service);
		CleCloseServer(server);
		free((void*)key);
		return 0;
	}
	thread = ThreadLayerCreateNewThread(dispatcher,server);
	
	printf("thread = %p\n",thread);
	
	if(!thread){
		free(service);
		CleCloseServer(server);
		free((void*)key);
		return 0;
	}
	service->thread_handle = thread;
	*service_ptr = service;
	return 1;
}

void* CleConnectServiceLowLevel(void* ctx,const char* name){
	void* handle;
	SERVICE* service = (SERVICE*) InternalContextAndNameGetDispatcher(ctx,name);
	if(!service) return 0;
	
	handle = CleCreateClient();
	if(!handle) return 0;
	
	if(!CleConnectClient(handle,service->key,service->key_len)){
		CleCloseClient(handle);
		return 0;
	}
	return handle;
}

