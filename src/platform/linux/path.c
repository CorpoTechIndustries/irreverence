#include <platform/path.h>
#include <platform/io.h>
#include <platform/memory.h>
#include <platform/dir.h>

#include <unistd.h>
#include <string.h>
#ifndef __USE_MISC
	#define __USE_MISC
#endif
#include <stdlib.h>
#include <linux/limits.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/dir.h>

int Sys_PathExists(const char* path)
{
	struct stat s;
	int res = stat(path, &s);

	if (res) {
		return 0;
	}

	return 1;
}

int Sys_PathIsFile(const char* path)
{
	struct stat s;
	int res = stat(path, &s);

	if (res) {
		return 0;
	}

	return S_ISREG(s.st_mode);
}

int Sys_PathIsDir(const char* path)
{
	struct stat s;
	int res = stat(path, &s);

	if (res) {
		return 0;
	}

	return S_ISDIR(s.st_mode);
}

int Sys_PathDeleteFile(const char* path)
{
	if (!Sys_PathIsFile(path)) {
		return 0;
	}

	int res = unlink(path);

	if (res) {
		return 0;
	}

	return 1;
}

int Sys_PathDeleteDir(const char* path)
{
	if (!Sys_PathIsDir(path)) {
		return 0;
	}

	int res = rmdir(path);

	if (res) {
		return 0;
	}

	return 1;
}

int Sys_PathCreateDir(const char* path)
{
	if (Sys_PathExists(path)) {
		return 0;
	}

	int res = mkdir(path, S_IRUSR | S_IWUSR);

	if (res) {
		return 0;
	}

	return 1;
}

static char real_path_buff[PATH_MAX];

int Sys_PathGetFullPath(const char* path, char* buff, size_t buff_size)
{
	if (!realpath(path, real_path_buff)) {
		return 0;
	}

	strncpy(buff, real_path_buff, buff_size);
	buff[buff_size] = 0;

	return 1;
}

const char* Sys_PathGetExtension(const char* path)
{
	size_t len = strlen(path);

	while (--len) {
		if (path[len] == '.') {
			break;
		}
	}

	return path + len + 1;
}

char* Sys_PathReadFile(const char* path, size_t* out_size)
{
	if (!Sys_PathIsFile(path)) {
		return NULL;
	}

	file_t file = Sys_FileOpen(path, FILE_MODE_READ);

	if (!file) {
		return NULL;
	}

	size_t size = Sys_FileSeek(file, FILE_SEEK_END, 0);

	if (out_size) {
		*out_size = size;
	}

	char* buff = Sys_Calloc(size + 1);

	Sys_FileSeek(file, FILE_SEEK_SET, 0);
	Sys_FileRead(file, buff, size);

	Sys_FileClose(file);

	return buff;
}

void Sys_PathIterateDir(const char* path, dir_iterator_t fn)
{
	dir_t* dir = Sys_DirOpen(path);

	if (!dir) {
		return;
	}

	dir_entry_t entry;

	while (Sys_DirRead(dir, &entry)) {
		if (strcmp(entry.name, ".") == 0 || strcmp(entry.name, "..") == 0) {
			continue;
		}

		fn(entry.name, entry.is_dir);
	}

	Sys_DirClose(dir);
}
