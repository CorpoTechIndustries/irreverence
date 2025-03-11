#pragma once

#define DIR_ENT_NAME_LEN 256
typedef struct {
	char name[DIR_ENT_NAME_LEN];
	int is_dir;
} dir_entry_t;

typedef struct dir_t dir_t;

dir_t* Sys_DirOpen(const char* path);
void Sys_DirClose(dir_t* dir);
int Sys_DirRead(dir_t* dir, dir_entry_t* entry);
