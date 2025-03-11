#pragma once

#include <stddef.h>

void* Sys_Malloc(size_t size);
void* Sys_Calloc(size_t size);
void Sys_Free(void* ptr);
void* Sys_ReAlloc(void* ptr, size_t new_size);

void Sys_MemZero(void* p, size_t size);
void Sys_MemSet(void* p, int value, size_t size);
void Sys_MemCpy(const void* src, void* dst, size_t size);
