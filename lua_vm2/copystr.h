#ifndef COPYSTR
#define COPYSTR
#include <stddef.h>

struct String_s;
typedef struct String_s String;
struct String_s{
	const char* ptr;
	size_t size;
};
int String_copy(String* dst,String src);
void String_drop(String* s);

#endif


