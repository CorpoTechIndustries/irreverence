#pragma once

#include <engine/renderer/renderer.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct framebuffer framebuffer_t;

#define MAX_TEXTURE_BINDS 16

typedef struct texture {
	uint32_t id;
	uint32_t width;
	uint32_t height;
	uint8_t channelCount;
} texture_t;

bool Texture_Init(texture_t* texture, const char* image_path, bool linearize, bool gen_mipmaps);
bool Texture_InitFromMemory(texture_t* texture, const uint8_t* data, uint32_t width, uint32_t height, uint8_t channel_count, bool linearize, bool gen_mipmaps);
bool Texture_InitColorAttachment(
	texture_t* texture, 
	framebuffer_t* framebuffer, 
	uint8_t location, 
	uint32_t width, 
	uint32_t height, 
	uint8_t samples, 
	gapi_enum_t format, 
	gapi_enum_t type);
bool Texture_InitDepthAttachment(
	texture_t* texture, 
	framebuffer_t* framebuffer, 
	uint32_t width, 
	uint32_t height, 
	uint8_t samples, 
	gapi_enum_t format, 
	gapi_enum_t type);
void Texture_Destroy(texture_t* texture);

void Texture_Bind(texture_t* texture, uint8_t location);
