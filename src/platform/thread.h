#pragma once

#include <stddef.h>

typedef size_t thread_t;
typedef void(*thread_fn_t)(thread_t thread, void* data);
thread_t Sys_ThreadCreate(thread_fn_t fn, const void* data, size_t data_size);
int Sys_ThreadKill(thread_t thread);
int Sys_ThreadJoin(thread_t thread);
int Sys_ThreadTryJoin(thread_t thread, size_t ms);
