#pragma once

#include <util/buffer.h>

typedef buffer_t net_buffer_t;

void NetBuffer_Create(net_buffer_t* buffer, size_t max_size);
void NetBuffer_CopyMemory(net_buffer_t* buffer, const void* ptr, size_t size);
#define NetBuffer_FromMemory(ptr, size) (net_buffer_t){ size, 0, ptr }
void NetBuffer_Free(net_buffer_t* buffer);

uint8_t NetBuffer_ReadUInt8(net_buffer_t* buffer);
uint16_t NetBuffer_ReadUInt16(net_buffer_t* buffer);
uint32_t NetBuffer_ReadUInt32(net_buffer_t* buffer);
float NetBuffer_ReadFloat(net_buffer_t* buffer);

void NetBuffer_WriteUInt8(net_buffer_t* buffer, uint8_t value);
void NetBuffer_WriteUInt16(net_buffer_t* buffer, uint16_t value);
void NetBuffer_WriteUInt32(net_buffer_t* buffer, uint32_t value);
void NetBuffer_WriteFloat(net_buffer_t* buffer, float value);
