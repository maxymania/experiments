/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
#include <COM/Cle64.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static int i;

int CleGenerateServiceKeyLocal(CLE_CONST_PTR* key,unsigned *key_len){
	char buffer[1024];
	void* dst;
	unsigned dl;
	sprintf(buffer,"inproc://#%d",++i);
	dl = strlen(buffer);
	dst = malloc(dl);
	if(!dst) return 0;
	memcpy(dst,buffer,dl);
	*key = dst;
	*key_len = dl;
	return 1;
}

