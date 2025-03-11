#pragma once

#include <stddef.h>

typedef void* mutex_t;

mutex_t Sys_MutexCreate();
void Sys_MutexDestroy(mutex_t mutex);
int Sys_MutexLock(mutex_t mutex);
int Sys_MutexTryLock(mutex_t mutex, size_t ms);
int Sys_MutexUnlock(mutex_t mutex);
