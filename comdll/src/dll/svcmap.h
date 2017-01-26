/*
 * Copyright (C) 2017 Simon Schmidt
 *
 * Usage of the works is permitted provided that this instrument is retained with
 * the works, so that any entity that uses the works is notified of this instrument.
 * DISCLAIMER: THE WORKS ARE WITHOUT WARRANTY.
 */
#pragma once

void** InternalContextAndNameCreateDispatcherSlot(void* ctx,const char* name);

void* InternalContextAndNameGetDispatcher(void* ctx,const char* name);

