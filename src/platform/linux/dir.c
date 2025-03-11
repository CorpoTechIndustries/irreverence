#include <platform/dir.h>
#include <platform/memory.h>

#include <stddef.h>
#include <string.h>

#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/syscall.h>

#ifndef __USE_MISC
	#define __USE_MISC
#endif

#include <sys/dir.h>

#include <stdio.h>

#define DIR_BUFFER_SIZE 2048
struct dir_t {
	int fd;
	int buf_pos;
	int buf_end;
	char buff[DIR_BUFFER_SIZE];
};

dir_t* Sys_DirOpen(const char* path)
{
	dir_t* dir = (dir_t*)Sys_Calloc(sizeof(*dir));

	dir->fd = open(path, __O_DIRECTORY | O_RDONLY | __O_CLOEXEC);

	if (dir->fd < 0) {
		Sys_Free(dir);
		return NULL;
	}

	return dir;
}

void Sys_DirClose(dir_t* dir)
{
	if (!dir) {
		return;
	}

	if (dir->fd < 0) {
		return;
	}

	close(dir->fd);
	Sys_Free(dir);
}

struct linux_dirent {
	unsigned long d_ino;
	unsigned long d_off;
	unsigned short d_reclen;
	char d_name[DIR_ENT_NAME_LEN];
	char pad;
	char d_type;
};

int Sys_DirRead(dir_t* dir, dir_entry_t* entry)
{
	if (dir->buf_pos >= dir->buf_end) {
		int res = syscall(SYS_getdents, dir->fd, dir->buff, sizeof(dir->buff));

		if (res <= 0) {
			return 0;
		}

		dir->buf_end = res;
		dir->buf_pos = 0;
	}

	struct linux_dirent* lent = (struct linux_dirent*)(dir->buff + dir->buf_pos);
	dir->buf_pos += lent->d_reclen;
	char* l = (char*)lent;
	char m = *(l + lent->d_reclen - 1);
	strncpy(entry->name, lent->d_name, DIR_ENT_NAME_LEN);
	entry->is_dir = m == DT_DIR;

	return 1;
}
