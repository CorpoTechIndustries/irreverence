#pragma once

#include <engine/renderer/renderer.h>

#include <stdint.h>
#include <stdbool.h>

#define MAX_TEXTURE_ARRAY_BINDS 16

typedef struct texture_array {
	uint32_t id;
	uint32_t width;
	uint32_t height;
	uint8_t channelCount;
} texture_array_t;

bool TextureArray_InitFromMemory(texture_array_t* tex_array, const uint8_t** data, uint8_t texture_count, uint32_t width, uint32_t height, uint8_t channel_count, bool linearize, bool gen_mipmaps);
void TextureArray_Destroy(texture_array_t* tex_array);

void TextureArray_Bind(texture_array_t* tex_array, uint8_t location);
