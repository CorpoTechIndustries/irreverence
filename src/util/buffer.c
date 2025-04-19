#include <util/buffer.h>

#include <platform/memory.h>

void Buffer_Create(buffer_t* buffer, size_t max_size)
{
	buffer->size = max_size;
	buffer->pos = 0;
	buffer->data = Sys_Calloc(max_size);
}

void Buffer_CopyMemory(buffer_t* buffer, const void* data, size_t size)
{
	buffer->size = size;
	buffer->pos = 0;
	buffer->data = Sys_Calloc(size);

	Sys_MemCpy(data, buffer->data, size);
}

void Buffer_Free(buffer_t* buffer)
{
	if (buffer->data) {
		Sys_Free(buffer->data);
		buffer->data = NULL;
	}
}

size_t Buffer_Read(buffer_t* buffer, void* dst, size_t size)
{
	if (buffer->pos + size > buffer->size) {
		size = (buffer->pos + size) - buffer->size;
	}

	const char* c_src = (const char*)buffer->data;

	Sys_MemCpy(c_src + buffer->pos, dst, size);

	buffer->pos += size;

	return size;
}

uint8_t Buffer_ReadUInt8(buffer_t* buffer)
{
	uint8_t v;

	Buffer_Read(buffer, &v, sizeof(v));

	return v;
}

uint16_t Buffer_ReadUInt16(buffer_t* buffer)
{
	uint16_t v;

	Buffer_Read(buffer, &v, sizeof(v));

	return v;
}

uint32_t Buffer_ReadUInt32(buffer_t* buffer)
{
	uint32_t v;

	Buffer_Read(buffer, &v, sizeof(v));

	return v;
}

uint64_t Buffer_ReadUInt64(buffer_t* buffer)
{
	uint64_t v;

	Buffer_Read(buffer, &v, sizeof(v));

	return v;
}

float Buffer_ReadFloat(buffer_t* buffer)
{
	float v;

	Buffer_Read(buffer, &v, sizeof(v));

	return v;
}

// FIXME: Fix this later on so that strings don't fuck up.
size_t Buffer_ReadString(buffer_t* buffer, char* buf, size_t maxSize)
{
	uint32_t size = Buffer_ReadUInt32(buffer);

	return Buffer_Read(buffer, buf, size);
}

size_t Buffer_Write(buffer_t* buffer, const void* data, size_t size)
{
	if (buffer->pos + size > buffer->size) {
		size = (buffer->pos + size) - buffer->size;
	}

	char* c_dst = buffer->data;

	Sys_MemCpy(data, c_dst + buffer->pos, size);

	buffer->pos += size;

	return size;
}

void Buffer_WriteUInt8(buffer_t* buffer, uint8_t value)
{
	Buffer_Write(buffer, &value, sizeof(value));
}

void Buffer_WriteUInt16(buffer_t* buffer, uint16_t value)
{
	Buffer_Write(buffer, &value, sizeof(value));
}

void Buffer_WriteUInt32(buffer_t* buffer, uint32_t value)
{
	Buffer_Write(buffer, &value, sizeof(value));
}

void Buffer_WriteUInt64(buffer_t* buffer, uint64_t value)
{
	Buffer_Write(buffer, &value, sizeof(value));
}

void Buffer_WriteFloat(buffer_t* buffer, float value)
{
	Buffer_Write(buffer, &value, sizeof(value));
}

void Buffer_WriteString(buffer_t* buffer, const char* text, size_t size)
{
	Buffer_WriteUInt32(buffer, size);
	Buffer_Write(buffer, text, size);
}
