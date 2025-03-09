#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint32_t id;
	size_t currentSize;
} storagebuffer_t;

void Storagebuffer_Init(storagebuffer_t* storagebuffer, void* data, size_t size);
void Storagebuffer_Destroy(storagebuffer_t* storagebuffer);

void Storagebuffer_Update(storagebuffer_t* storagebuffer, void* data);