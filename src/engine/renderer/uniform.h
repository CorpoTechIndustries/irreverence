#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint32_t id;
	uint8_t location;
	size_t currentSize;
} uniform_t;

bool Uniform_Init(uniform_t* uniform, uint8_t location, void* data, size_t size);
void Uniform_Destroy(uniform_t* uniform);

void Uniform_Update(uniform_t* uniform, void* data, size_t data_size, size_t offset);
void Uniform_Bind(uniform_t* uniform);
void Uniform_Relocate(uniform_t* uniform, uint8_t location);
