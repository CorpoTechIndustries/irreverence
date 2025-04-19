#include <platform/io.h>
#include <platform/mutex.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/syscall.h>
#include <sys/stat.h>

#define STDOUT 0
#define STDIN 1
#define STDERR 2

static mutex_t write_mutex;
static mutex_t format_mutex;

void Sys_IOInit()
{
	write_mutex = Sys_MutexCreate();
	format_mutex = Sys_MutexCreate();
}

void Sys_IOClose()
{
	Sys_MutexDestroy(write_mutex);
	Sys_MutexDestroy(format_mutex);
}

void Sys_Print(const char* str)
{
	if (!Sys_MutexLock(write_mutex)) {
		return;
	}

	Sys_FilePrint(STDOUT, str);

	Sys_MutexUnlock(write_mutex);
}

#define FORMAT_BUFFER_SIZE 30000

static char format_buffer[FORMAT_BUFFER_SIZE];

void Sys_Printf(const char* fmt, ...)
{
	if (!Sys_MutexLock(format_mutex)) {
		return;
	}

	va_list l;

	va_start(l, fmt);

	vsnprintf(format_buffer, FORMAT_BUFFER_SIZE, fmt, l);
	Sys_Print(format_buffer);

	va_end(l);

	Sys_MutexUnlock(format_mutex);
}

void Sys_VPrintf(const char* fmt, va_list args)
{
	vsnprintf(format_buffer, FORMAT_BUFFER_SIZE, fmt, args);
	Sys_Print(format_buffer);
}

file_t Sys_FileOpen(const char* path, enum file_mode_t mode)
{
	int flags = 0;

	if (mode & FILE_MODE_CREATE) {
		flags |= O_CREAT;
	}

	if (mode & FILE_MODE_TRUNC) {
		flags |= O_TRUNC;
	}

	if (mode & FILE_MODE_APPEND) {
		flags |= O_APPEND;
	}

	int rd = mode & FILE_MODE_READ;
	int wr = mode & FILE_MODE_WRITE;

	if (rd && wr) {
		flags |= O_RDWR;
	} else if (rd) {
		flags |= O_RDONLY;
	} else if (wr) {
		flags |= O_WRONLY;
	}

	return open(path, flags, S_IRUSR | S_IWUSR);
}

void Sys_FileClose(file_t file)
{
	close(file);
}

size_t Sys_FileSeek(file_t file, enum file_seek_t seek, size_t pos)
{
	lseek(file, pos, seek);
}

size_t Sys_FileTell(file_t file)
{
	return Sys_FileSeek(file, FILE_SEEK_CUR, 0);
}

size_t Sys_FileRead(file_t file, void* buff, size_t buff_size)
{
	return read(file, buff, buff_size);
}

size_t Sys_FileWrite(file_t file, const void* buff, size_t buff_size)
{
	return write(file, buff, buff_size);
}

size_t Sys_FilePrint(file_t file, const char* str)
{
	size_t len = strlen(str);

	return Sys_FileWrite(file, str, len);
}

size_t Sys_FilePrintf(file_t file, const char* fmt, ...)
{
	va_list l;

	va_start(l, fmt);
	vsnprintf(format_buffer, FORMAT_BUFFER_SIZE, fmt, l);
	size_t i = Sys_FilePrint(file, format_buffer);
	va_end(l);

	return i;
}

size_t Sys_FileVPrintf(file_t file, const char* fmt, va_list args)
{
	vsnprintf(format_buffer, FORMAT_BUFFER_SIZE, fmt, args);

	return Sys_FilePrint(file, format_buffer);
}

uint8_t Sys_FileReadUInt8(file_t file)
{
	uint8_t v;

	Sys_FileRead(file, &v, sizeof(v));

	return v;
}

uint16_t Sys_FileReadUInt16(file_t file)
{
	uint16_t v;

	Sys_FileRead(file, &v, sizeof(v));

	return v;
}

uint32_t Sys_FileReadUInt32(file_t file)
{
	uint32_t v;

	Sys_FileRead(file, &v, sizeof(v));

	return v;
}

uint64_t Sys_FileReadUInt64(file_t file)
{
	uint64_t v;

	Sys_FileRead(file, &v, sizeof(v));

	return v;
}

float Sys_FileReadFloat(file_t file)
{
	float v;

	Sys_FileRead(file, &v, sizeof(v));

	return v;
}

void Sys_FileWriteUInt8(file_t file, uint8_t value)
{
	Sys_FileWrite(file, &value, sizeof(value));
}

void Sys_FileWriteUInt16(file_t file, uint16_t value)
{
	Sys_FileWrite(file, &value, sizeof(value));
}

void Sys_FileWriteUInt32(file_t file, uint32_t value)
{
	Sys_FileWrite(file, &value, sizeof(value));
}

void Sys_FileWriteUInt64(file_t file, uint64_t value)
{
	Sys_FileWrite(file, &value, sizeof(value));
}

void Sys_FileWriteFloat(file_t file, float value)
{
	Sys_FileWrite(file, &value, sizeof(value));
}
