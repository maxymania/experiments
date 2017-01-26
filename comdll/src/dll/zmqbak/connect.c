/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
#include <zmq.h>
#include <stdlib.h>
#include <string.h>
#include <COM/Cle64.h>

int CleBindServer(void* server,const void* key,unsigned key_len){
	int n;
	char* temp = malloc(key_len+1);
	if(temp==0) return 0;
	memcpy(temp,key,key_len);
	temp[key_len] = 0;
	n = zmq_bind(server,temp);
	free(temp);
	return !n;
}

int CleConnectClient(void* client,const void* key,unsigned key_len){
	int n;
	char* temp = malloc(key_len+1);
	if(temp==0) return 0;
	memcpy(temp,key,key_len);
	temp[key_len] = 0;
	n = zmq_connect(client,temp);
	free(temp);
	return !n;
}

