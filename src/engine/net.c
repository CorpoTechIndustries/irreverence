#include <engine/net.h>
#include <util/endian.h>

void NetBuffer_Create(net_buffer_t* buffer, size_t max_size)
{
	Buffer_Create(buffer, max_size);
}

void NetBuffer_CopyMemory(net_buffer_t* buffer, const void* ptr, size_t size)
{
	Buffer_CopyMemory(buffer, ptr, size);
}

void NetBuffer_Free(net_buffer_t* buffer)
{
	Buffer_Free(buffer);
}

uint8_t NetBuffer_ReadUInt8(net_buffer_t* buffer)
{
	return Buffer_ReadUInt8(buffer);
}

uint16_t NetBuffer_ReadUInt16(net_buffer_t* buffer)
{
	uint16_t value = Buffer_ReadUInt16(buffer);

	return Endian_BigToHost16(value);
}

uint32_t NetBuffer_ReadUInt32(net_buffer_t* buffer)
{
	uint32_t value = Buffer_ReadUInt32(buffer);

	return Endian_BigToHost32(value);
}

float NetBuffer_ReadFloat(net_buffer_t* buffer)
{
	uint32_t value = NetBuffer_ReadUInt32(buffer);

	return *(float*)&value;
}

void NetBuffer_WriteUInt8(net_buffer_t* buffer, uint8_t value)
{
	Buffer_WriteUInt8(buffer, value);
}

void NetBuffer_WriteUInt16(net_buffer_t* buffer, uint16_t value)
{
	Buffer_WriteUInt16(buffer, Endian_HostToBig16(value));
}

void NetBuffer_WriteUInt32(net_buffer_t* buffer, uint32_t value)
{
	Buffer_WriteUInt32(buffer, Endian_HostToBig32(value));
}

void NetBuffer_WriteFloat(net_buffer_t* buffer, float value)
{
	NetBuffer_WriteUInt32(buffer, *(uint32_t*)&value);
}
