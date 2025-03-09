#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint32_t id;
	size_t currentSize;
} uniform_t;

void Uniform_Init(uniform_t* uniform, uint8_t location, void* data, size_t data_size);
void Uniform_Destroy(uniform_t* uniform);

void Uniform_Update(uniform_t* uniform, void* data);
void Uniform_Relocate(uniform_t* uniform, uint8_t location);