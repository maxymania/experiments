#include <stdlib.h>
#include <string.h>
#include "copystr.h"

int String_copy(String* dst,String src){
	if(dst->ptr)free((void*)(dst->ptr));
	dst->ptr = NULL;
	if(dst->size = src.size){
		dst->ptr = malloc(src.size);
		if(dst->ptr){
			memcpy((void*)(dst->ptr),(void*)src.ptr,src.size);
		}else{
			dst->size = 0;
			return 0;
		}
	}
	return 1;
}
void String_drop(String* s){
	if(s->ptr)free((void*)(s->ptr));
	s->ptr = NULL;
	s->size = 0;
}


