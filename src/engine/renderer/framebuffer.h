#pragma once

#include <engine/renderer/texture.h>

#include <stdint.h>

#define MAX_FRAMEBUFFER_COLOR 8

typedef struct {
	uint32_t format;
	uint32_t type;
} framebuffer_attachment_t;

typedef struct Framebuffer {
	uint32_t id;
	uint32_t rbo;
	uint32_t width;
	uint32_t height;
	texture_t colorTextures[MAX_FRAMEBUFFER_COLOR];
	texture_t depthTexture;
} framebuffer_t;

void Framebuffer_Init(
	framebuffer_t* framebuffer, 
	uint32_t width, 
	uint32_t height,
	uint8_t samples,
	framebuffer_attachment_t* attachments, 
	uint8_t attachment_count,
	framebuffer_attachment_t* depth_attachment);
void Framebuffer_Destroy(framebuffer_t* framebuffer);

void Framebuffer_CopyTo(framebuffer_t* framebuffer, framebuffer_t* to, bool include_depth);
void Framebuffer_Bind(framebuffer_t* framebuffer);
void Framebuffer_UnBind();