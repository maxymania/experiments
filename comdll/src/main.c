/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <COM/idr.h>
#include <COM/Cle64.h>
#include <COM/Coidr.h>

void server(void* srv){
	char buffer[1024];
	int n;
	IDR idr;
	int32_t I;
	int64_t Q;
	
	
	for(;;){
		idr = IDR_constructor(buffer,sizeof buffer);
		CoIdrRecv(srv,&idr);
		
		IDR_hyper_integer_r(&idr,&Q);
		IDR_integer_r(&idr,&I);
		
		printf("Request %p, %d\n",(void*)Q,I);
		
		idr = IDR_constructor(buffer,sizeof buffer);
		IDR_integer_w(&idr,I);
		
		CoIdrSend(srv,&idr);
	}
}

int main(){
	
	char buffer[1024];
	int n,i;
	IDR idr;
	int32_t I;
	pthread_t t;
	
	CleInitBackEnd();
	
	CleCreateServiceLowLevel(NULL,server,"My Service No. 1");
	
	void* conn = CleConnectServiceLowLevel(NULL,"My Service No. 1");
	
	i=0;
	
	for(;;){
		idr = IDR_constructor(buffer,sizeof buffer);
		
		IDR_hyper_integer_w(&idr,0);
		IDR_integer_w(&idr,i++);
		
		CoIdrSend(conn,&idr);
		
		idr = IDR_constructor(buffer,sizeof buffer);
		CoIdrRecv(conn,&idr);
		
		IDR_integer_r(&idr,&I);
		
		printf("Response %d\n",I);
	}
}

