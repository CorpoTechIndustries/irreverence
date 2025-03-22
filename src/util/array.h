#pragma once

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* array_t;

array_t Array_CreateStride(size_t stride, size_t size);

#define Array_CreateSize(__type, __size) \
(__type*)Array_CreateStride(sizeof(__type), __size)

#define ARRAY_DEFAULT_SIZE 16

#define Array_Create(__type) \
Array_CreateSize(__type, ARRAY_DEFAULT_SIZE)

void Array_Destroy(array_t array);

size_t Array_Stride(array_t array);
size_t Array_Size(array_t array);
size_t Array_Count(array_t array);
array_t Array_Resize(array_t array, size_t size);
void* Array_Offset(array_t array, size_t offset);
void Array_Copy(array_t src, array_t dst, size_t count);

array_t Array_PushData(array_t array, const void* data);
void Array_PopData(array_t array, void* dst);

#define Array_Push(__arr, __v) \
do { typeof(__v) __var = __v; __arr = Array_PushData(__arr, &__var); } while(0)

#define Array_Pop(__arr, __var) \
do { Array_PopData(__arr, &__var); } while(0)

void Array_SetData(array_t array, size_t index, const void* data);
void Array_GetData(array_t array, size_t index, void* dst);

#define Array_Set(__arr, __ind, __v) \
do { typeof(__v) __var = __v; Array_SetData(__arr, __ind, &__var); } while(0)

#define Array_Get(__arr, __ind, __var) \
do { Array_GetData(__arr, __ind, &__var); } while(0)

typedef void(*array_iterator_t)(size_t index, void* value);
void Array_Iterate(array_t array, array_iterator_t fn);
void Array_IterateCount(array_t array, array_iterator_t fn);

#ifdef __cplusplus
}
#endif
