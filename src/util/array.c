#include <util/array.h>
#include <platform/memory.h>
#include <stdalign.h>

struct array_header_t {
	alignas(16) size_t stride;
	size_t size;
	size_t count;
};

array_t Array_CreateStride(size_t stride, size_t size)
{
	struct array_header_t* header = Sys_Calloc(sizeof(*header) + (stride * size));

	header->stride = stride;
	header->size = size;
	header->count = 0;

	return (array_t)(header + 1);
}

static struct array_header_t* get_header(array_t array)
{
	struct array_header_t* header = (struct array_header_t*)array;

	return header - 1;
}

void Array_Destroy(array_t array)
{
	struct array_header_t* header = get_header(array);

	Sys_Free(header);
}

size_t Array_Stride(array_t array)
{
	return get_header(array)->stride;
}

size_t Array_Size(array_t array)
{
	return get_header(array)->size;
}

size_t Array_Count(array_t array)
{
	return get_header(array)->count;
}

array_t Array_Resize(array_t array, size_t size)
{
	struct array_header_t* header = get_header(array);

	header = Sys_ReAlloc(header, sizeof(*header) + (header->stride * size));
	header->size = size;

	if (header->count >= header->size) {
		header->count = header->size;
	}

	char* ptr = (char*)(header + 1);

	Sys_MemZero(ptr + header->count * header->stride, (size - header->count) * header->stride);

	return (array_t)ptr;
}

void Array_Clear(array_t array)
{
	struct array_header_t* header = get_header(array);
	header->count = 0;

	Sys_MemZero(array, header->size);
}

void* Array_Offset(array_t array, size_t offset)
{
	size_t stride = Array_Stride(array);

	char* ptr = (char*)array;

	return ptr + (stride * offset);
}

array_t Array_PushData(array_t array, const void* data)
{
	struct array_header_t* header = get_header(array);

	if (header->count + 1 >= header->size) {
		array = Array_Resize(array, header->size * 1.5f);
		return Array_PushData(array, data);
	}

	char* arr_c = (char*)array;
	char* data_c = (char*)data;

	arr_c += (header->count++ * header->stride);

	for (size_t i = 0; i < header->stride; i++) {
		arr_c[i] = data_c[i];
	}

	return array;
}

void Array_PopData(array_t array, void* dst)
{
	struct array_header_t* header = get_header(array);

	if (header->count < 1) {
		return;
	}

	char* arr_c = (char*)array;
	char* dst_c = (char*)dst;

	arr_c += (--header->count * header->stride);

	for (size_t i = 0; i < header->stride; i++) {
		dst_c[i] = arr_c[i];
	}
}

void Array_SetData(array_t array, size_t index, const void* data)
{
	struct array_header_t* header = get_header(array);

	if (index >= header->size) {
		return;
	}

	char* arr_c = (char*)array;
	char* data_c = (char*)data;

	arr_c += (index * header->stride);

	for (size_t i = 0; i < header->stride; i++) {
		arr_c[i] = data_c[i];
	}
}

void Array_GetData(array_t array, size_t index, void* dst)
{
	struct array_header_t* header = get_header(array);

	if (index >= header->size) {
		return;
	}

	char* arr_c = (char*)array;
	char* dst_c = (char*)dst;

	arr_c += (index * header->stride);

	for (size_t i = 0; i < header->stride; i++) {
		dst_c[i] = arr_c[i];
	}
}

void Array_Iterate(array_t array, array_iterator_t fn)
{
	struct array_header_t* header = get_header(array);

	char* arr_c = (char*)array;

	for (size_t i = 0; i < header->size; i++) {
		fn(i, (void*)arr_c);
		arr_c += header->stride;
	}
}

void Array_IterateCount(array_t array, array_iterator_t fn)
{
	struct array_header_t* header = get_header(array);

	char* arr_c = (char*)array;

	for (size_t i = 0; i < header->count; i++) {
		fn(i, (void*)arr_c);
		arr_c += header->stride;
	}
}
