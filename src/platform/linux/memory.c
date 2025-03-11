#include <platform/memory.h>

// Just using malloc and free for now, might write our own allocator later down
// the line.

#include <stdlib.h>
#include <string.h>

void* Sys_Malloc(size_t size)
{
	return malloc(size);
}

void* Sys_Calloc(size_t size)
{
	void* p = Sys_Malloc(size);

	Sys_MemZero(p, size);

	return p;
}

void Sys_Free(void* ptr)
{
	free(ptr);
}

void* Sys_ReAlloc(void* ptr, size_t new_size)
{
	return realloc(ptr, new_size);
}

void Sys_MemZero(void* p, size_t size)
{
	Sys_MemSet(p, 0, size);
}

void Sys_MemSet(void* p, int value, size_t size)
{
	char* dst = (char*)p;

	for (size_t i = 0; i < size; i++) {
		dst[i] = value;
	}
}

void Sys_MemCpy(const void* src, void* dst, size_t size)
{
	const char* c_src = (const char*)src;
	char* c_dst = (char*)dst;

	for (size_t i = 0; i < size; i++) {
		c_dst[i] = c_src[i];
	}
}
