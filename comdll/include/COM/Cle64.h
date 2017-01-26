/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
#pragma once

typedef void (*CLE_SERVICE_ROUTINE)(void* sock);
typedef const void* CLE_CONST_PTR;

int CleInitBackEnd() __attribute__ ((visibility("default")));

void* CleCreateServer() __attribute__ ((visibility("default")));

void* CleCreateClient() __attribute__ ((visibility("default")));

void CleCloseClient(void* handle) __attribute__ ((visibility("default")));

void CleCloseServer(void* handle) __attribute__ ((visibility("default")));

int CleGenerateServiceKeyLocal(CLE_CONST_PTR* key,unsigned *key_len)
	__attribute__ ((visibility("default")));

int CleBindServer(void* server,const void* key,unsigned key_len)
	__attribute__ ((visibility("default")));

int CleConnectClient(void* client,const void* key,unsigned key_len)
	__attribute__ ((visibility("default")));

int CleCreateServiceLowLevel(void* ctx,CLE_SERVICE_ROUTINE dispatcher,const char* name)
	__attribute__ ((visibility("default")));

void* CleConnectServiceLowLevel(void* ctx,const char* name)
	__attribute__ ((visibility("default")));

