#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	size_t size;
	size_t pos;
	void* data;
} buffer_t;

void Buffer_Create(buffer_t* buffer, size_t max_size);
void Buffer_CopyMemory(buffer_t* buffer, const void* ptr, size_t size);
#define Buffer_FromMemory(ptr, size) (buffer_t){ size, 0, ptr }
void Buffer_Free(buffer_t* buffer);

size_t Buffer_Read(buffer_t* buffer, void* dst, size_t size);
uint8_t Buffer_ReadUInt8(buffer_t* buffer);
uint16_t Buffer_ReadUInt16(buffer_t* buffer);
uint32_t Buffer_ReadUInt32(buffer_t* buffer);
uint64_t Buffer_ReadUInt64(buffer_t* buffer);
float Buffer_ReadFloat(buffer_t* buffer);
size_t Buffer_ReadString(buffer_t* buffer, char* buf, size_t maxSize);

size_t Buffer_Write(buffer_t* buffer, const void* data, size_t size);
void Buffer_WriteUInt8(buffer_t* buffer, uint8_t value);
void Buffer_WriteUInt16(buffer_t* buffer, uint16_t value);
void Buffer_WriteUInt32(buffer_t* buffer, uint32_t value);
void Buffer_WriteUInt64(buffer_t* buffer, uint64_t value);
void Buffer_WriteFloat(buffer_t* buffer, float value);
void Buffer_WriteString(buffer_t* buffer, const char* text, size_t size);

#ifdef __cplusplus
}
#endif
