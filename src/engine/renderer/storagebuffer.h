#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	uint32_t id;
	bool realloc;
	uint8_t location;
	size_t currentSize;
} storagebuffer_t;

void Storagebuffer_Init(storagebuffer_t* storagebuffer, uint8_t location, void* data, size_t size, bool realloc);
void Storagebuffer_Destroy(storagebuffer_t* storagebuffer);

void Storagebuffer_Update(storagebuffer_t* storagebuffer, void* data, size_t data_size, size_t offset);