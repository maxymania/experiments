/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
//#include <COM/Cle64.h>
extern "C" {
#include "svcmap.h"
};
#include <map>
#include <string>

namespace{
class ContextObject{
public:
	std::map<std::string,void*> slotmap;
};
};

static ContextObject* context_object = 0;

static ContextObject* getContextObject(){
	if(context_object==0)
		context_object = new ContextObject;
	return context_object;
}

void** InternalContextAndNameCreateDispatcherSlot(void* ctx,const char* name){
	std::string str = name;
	ContextObject* context = ctx? reinterpret_cast<ContextObject*>(ctx) : getContextObject();
	return &(context->slotmap[str]);
}

void* InternalContextAndNameGetDispatcher(void* ctx,const char* name){
	std::string str = name;
	ContextObject* context = ctx? reinterpret_cast<ContextObject*>(ctx) : getContextObject();
	std::map<std::string,void*>::const_iterator iter = context->slotmap.find(name), end = context->slotmap.end();
	if(iter==end)
		return NULL;
	else
		return iter->second;
}

