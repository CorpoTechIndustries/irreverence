#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MAX_TEXTURE_BINDS 16

typedef struct {
	uint32_t id;
	uint32_t width;
	uint32_t height;
	uint8_t channelCount;
} texture_t;

void Texture_InitFromMemory(texture_t* texture, const uint8_t* data, uint32_t width, uint32_t height, uint8_t channel_count, bool linearize, bool gen_mipmaps);
void Texture_InitFromImage(texture_t* texture, const char* image_path, bool linearize, bool gen_mipmaps);
void Texture_Destroy(texture_t* texture);

void Texture_Bind(texture_t* texture, uint8_t location);