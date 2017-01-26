/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
#include <zmq.h>
#include <COM/Cle64.h>

static void *context;

int CleInitBackEnd(){
	context = zmq_ctx_new();
	return context!=0;
}

void* CleCreateServer(){
	return zmq_socket(context, ZMQ_REP);
}

void* CleCreateClient(){
	return zmq_socket(context, ZMQ_REQ);
}

void CleCloseClient(void* handle){
	zmq_close(handle);
}

void CleCloseServer(void* handle){
	zmq_close(handle);
}
