/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
#pragma once
#include <stdint.h>
#include <string.h>

/*
 * IDR - Internal Data Representation
 *  aka. IPC Data Representation
 */

typedef struct{
	uint8_t* buffer;
	int32_t  size;
	int32_t  initsz;
} IDR;

static inline IDR IDR_constructor(void* buf,int32_t size){ return (IDR){buf,size,size}; }

static inline uint32_t IDR_writesize(IDR *idr){
	return (idr->initsz) -= (idr->size);
}
static inline void* IDR_writeptr(IDR *idr){
	return (idr->buffer) -= (idr->initsz);
}

static inline uint32_t IDR_mod4up(uint32_t n){
	uint32_t m = n%4;
	return n - m + (m?4:0);
}

#define IDR_API(tyname,tyc)                   \
static inline int                             \
IDR_ ## tyname ## _r (IDR *idr,tyc *i){       \
	if(idr->size < sizeof(tyc)) return 0; \
	*i = *((tyc*)(idr->buffer));          \
	idr->buffer += sizeof(tyc);           \
	idr->size   -= sizeof(tyc);           \
	return 1;                             \
}                                             \
static inline int                             \
IDR_ ## tyname ## _w (IDR *idr,tyc i){        \
	if(idr->size < sizeof(tyc)) return 0; \
	*((tyc*)(idr->buffer)) = i;           \
	idr->buffer += sizeof(tyc);           \
	idr->size   -= sizeof(tyc);           \
	return 1;                             \
}

IDR_API(bool,int32_t)
IDR_API(enum,int32_t)
IDR_API(integer,int32_t)
IDR_API(unsigned,uint32_t)
IDR_API(hyper_integer,int64_t)
IDR_API(hyper_unsigned,int64_t)
IDR_API(float,float)
IDR_API(double,double)
#undef IDR_API

static inline int
IDR_opaque_r(IDR *idr,uint8_t *buf,uint32_t size){
	uint32_t inc = IDR_mod4up(size);
	if(idr->size<inc) return 0;
	memcpy(buf,idr->buffer,size);
	idr->buffer += inc;
	idr->size   -= inc;
	return 0;
}

static inline int
IDR_opaque_w(IDR *idr,uint8_t *buf,uint32_t size){
	uint32_t inc = IDR_mod4up(size);
	if(idr->size<inc) return 0;
	memcpy(idr->buffer,buf,size);
	idr->buffer += inc;
	idr->size   -= inc;
	return 0;
}

static inline int
IDR_var_opaque_r(IDR *idr,uint8_t *buf,uint32_t *cusize,uint32_t maxsize){
	if(idr->size < sizeof(uint32_t)) return 0;
	uint32_t size = *((uint32_t*)(idr->buffer));
	uint32_t inc  = IDR_mod4up(size);
	if(idr->size<(inc+sizeof(uint32_t))) return 0;
	idr->buffer += sizeof(uint32_t);
	idr->size   -= sizeof(uint32_t);
	memcpy(buf,idr->buffer,size);
	idr->buffer += inc;
	idr->size   -= inc;
	return 0;
}

static inline int
IDR_var_opaque_w(IDR *idr,uint8_t *buf,uint32_t *cusize,uint32_t maxsize){
	if(idr->size < sizeof(uint32_t)) return 0;
	uint32_t size = *((uint32_t*)(idr->buffer));
	uint32_t inc  = IDR_mod4up(size);
	if(idr->size<(inc+sizeof(uint32_t))) return 0;
	idr->buffer += sizeof(uint32_t);
	idr->size   -= sizeof(uint32_t);
	memcpy(idr->buffer,buf,size);
	idr->buffer += inc;
	idr->size   -= inc;
	return 0;
}

