#pragma once

#include <stddef.h>

int Sys_PathExists(const char* path);
int Sys_PathIsFile(const char* path);
int Sys_PathIsDir(const char* path);
int Sys_PathDeleteFile(const char* path);
int Sys_PathDeleteDir(const char* path);
int Sys_PathCreateDir(const char* path);
int Sys_PathGetFullPath(const char* path, char* buff, size_t buff_size);

char* Sys_PathReadFile(const char* path, size_t* size);

typedef void(*dir_iterator_t)(const char* path, int is_dir);
void Sys_PathIterateDir(const char* path, dir_iterator_t fn);
