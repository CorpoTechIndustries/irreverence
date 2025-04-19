#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
	size_t currentSize;
	void* map;
	uint32_t id;
	bool realloc;
	uint8_t location;
} storagebuffer_t;

bool Storagebuffer_Init(storagebuffer_t* storagebuffer, uint8_t location, void* data, size_t size, bool realloc);
void Storagebuffer_Destroy(storagebuffer_t* storagebuffer);

void Storagebuffer_Update(storagebuffer_t* storagebuffer, void* data, size_t data_size, size_t offset);
