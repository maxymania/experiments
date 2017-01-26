/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
#include <zmq.h>
#include <COM/Coidr.h>

int CoIdrSend(void* conn,IDR* idr){
	return zmq_send(conn,IDR_writeptr(idr),IDR_writesize(idr),0) >= 0;
}

int CoIdrRecv(void* conn,IDR* idr){
	int n = zmq_recv(conn,idr->buffer,idr->size,0);
	if(n<=0) return 0;
	idr->size = n;
	idr->initsz = n;
	return 1;
}

