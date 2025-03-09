#pragma once

#include <engine/renderer/texture.h>

#include <stdint.h>

#define MAX_FRAMEBUFFER_COLOR 8

typedef struct {
	uint32_t id;
	uint32_t rboId;
	uint32_t width;
	uint32_t height;
	texture_t colorTextures[MAX_FRAMEBUFFER_COLOR];
	texture_t depthTexture;
} framebuffer_t;

void Framebuffer_Init(framebuffer_t* framebuffer, uint32_t width, uint32_t height);
void Framebuffer_Destroy(framebuffer_t* framebuffer);

void Framebuffer_Bind(framebuffer_t* framebuffer);
void Framebuffer_UnBind();