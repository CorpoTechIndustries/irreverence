#pragma once

#include <util/buffer.h>
#include <stddef.h>
#include <stdbool.h>
#include <platform/udp.h>

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

typedef udp_address_t net_address_t;

typedef enum {
	NET_SERVER,
	NET_CLIENT,
	NET_COUNT
} net_source_t;

#define MAX_NET_PACKET_SIZE 2 << 16

bool Net_Init();
void Net_Close();
bool Net_Setup(bool multiplayer);
int Net_SendPacket(net_source_t source, const void* data, size_t size, net_address_t address);
int Net_ReadPacket(net_source_t source, void* data, size_t size, net_address_t* from);

net_address_t Net_LocalAddress(uint16_t port);
net_address_t Net_Address(const char* address, uint16_t port);
const char* Net_AddressToString(net_address_t address, uint16_t* port);
