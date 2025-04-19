#pragma once

#include <stddef.h>

int Sys_PathExists(const char* path);
int Sys_PathIsFile(const char* path);
int Sys_PathIsDir(const char* path);
int Sys_PathDeleteFile(const char* path);
int Sys_PathDeleteDir(const char* path);
int Sys_PathCreateDir(const char* path);
int Sys_PathGetFullPath(const char* path, char* buff, size_t buff_size);
const char* Sys_PathGetExtension(const char* path);

char* Sys_PathReadFile(const char* path, size_t* size);

typedef void(*dir_iterator_t)(const char* path, int is_dir);
void Sys_PathIterateDir(const char* path, dir_iterator_t fn);

typedef enum {
	WATCH_CREATE = 1 << 0,
	WATCH_MODIFY = 1 << 1,
	WATCH_DELETE = 1 << 2,
} watch_flags_t;

typedef int watch_fd_t;

typedef void(*watch_callback_t)(const char* path, int is_dir, watch_flags_t flag);

watch_fd_t Sys_PathWatch(const char* path, watch_flags_t flags, watch_callback_t cb);
void Sys_PathUnWatch(watch_fd_t fd);
