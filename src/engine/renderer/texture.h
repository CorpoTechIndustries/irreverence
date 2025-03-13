#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct Framebuffer framebuffer_t;

#define MAX_TEXTURE_BINDS 16

typedef struct {
	uint32_t id;
	uint32_t width;
	uint32_t height;
	uint8_t channelCount;
} texture_t;

void Texture_Init(texture_t* texture, const char* image_path, bool linearize, bool gen_mipmaps);
void Texture_InitFromMemory(texture_t* texture, const uint8_t* data, uint32_t width, uint32_t height, uint8_t channel_count, bool linearize, bool gen_mipmaps);
void Texture_InitColorAttachment(
	texture_t* texture, 
	framebuffer_t* framebuffer, 
	uint8_t location, 
	uint32_t width, 
	uint32_t height, 
	uint8_t samples, 
	uint32_t format, 
	uint32_t type);
void Texture_InitDepthAttachment(
	texture_t* texture, 
	framebuffer_t* framebuffer, 
	uint32_t width, 
	uint32_t height, 
	uint8_t samples, 
	uint32_t format, 
	uint32_t type);
void Texture_Destroy(texture_t* texture);

void Texture_Bind(texture_t* texture, uint8_t location);