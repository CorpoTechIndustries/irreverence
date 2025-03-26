#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

void Sys_Print(const char* str);
void Sys_Printf(const char* fmt, ...);
void Sys_VPrintf(const char* fmt, va_list args);

typedef int file_t;

enum file_mode_t {
	FILE_MODE_READ = 1 << 0,
	FILE_MODE_WRITE = 1 << 1,
	FILE_MODE_APPEND = 1 << 2,
	FILE_MODE_CREATE = 1 << 3,
	FILE_MODE_TRUNC = 1 << 4
};

enum file_seek_t {
	FILE_SEEK_SET,
	FILE_SEEK_CUR,
	FILE_SEEK_END
};

file_t Sys_FileOpen(const char* path, enum file_mode_t mode);
void Sys_FileClose(file_t file);
size_t Sys_FileSeek(file_t file, enum file_seek_t seek, size_t pos);
size_t Sys_FileTell(file_t file);
size_t Sys_FileRead(file_t file, void* buff, size_t buff_size);
size_t Sys_FileWrite(file_t file, const void* data, size_t data_size);
size_t Sys_FilePrint(file_t file, const char* str);
size_t Sys_FilePrintf(file_t file, const char* fmt, ...);
size_t Sys_FileVPrintf(file_t file, const char* fmt, va_list args);

uint8_t Sys_FileReadUInt8(file_t file);
uint16_t Sys_FileReadUInt16(file_t file);
uint32_t Sys_FileReadUInt32(file_t file);
uint64_t Sys_FileReadUInt64(file_t file);
float Sys_FileReadFloat(file_t file);

void Sys_FileWriteUInt8(file_t file, uint8_t value);
void Sys_FileWriteUInt16(file_t file, uint16_t value);
void Sys_FileWriteUInt32(file_t file, uint32_t value);
void Sys_FileWriteUInt64(file_t file, uint64_t value);
void Sys_FileWriteFloat(file_t file, float value);
