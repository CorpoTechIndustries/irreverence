#pragma once

#include <engine/renderer/renderer.h>
#include <engine/renderer/texture.h>

#include <math/vec4.h>

#include <stdint.h>
#include <stdbool.h>

#define MAX_FRAMEBUFFER_COLOR 8

typedef struct {
	gapi_enum_t format;
	gapi_enum_t type;
	uint8_t maxMips; // Only for Color attachments
} framebuffer_attachment_t;

typedef struct framebuffer {
	uint32_t id;
	uint32_t rbo;
	uint32_t width;
	uint32_t height;
	texture_t colorTextures[MAX_FRAMEBUFFER_COLOR];
	texture_t depthTexture;
} framebuffer_t;

bool Framebuffer_Init(
	framebuffer_t* framebuffer, 
	uint32_t width, 
	uint32_t height,
	uint8_t samples,
	framebuffer_attachment_t* attachments, 
	uint8_t attachment_count,
	framebuffer_attachment_t* depth_attachment);
void Framebuffer_Destroy(framebuffer_t* framebuffer);

void Framebuffer_CopyTo(framebuffer_t* framebuffer, framebuffer_t* to, uint8_t attachmentId);
void Framebuffer_ClearColor(framebuffer_t* framebuffer, uint8_t attachment, const vec4_t color);
void Framebuffer_RelinkAttachment(framebuffer_t* framebuffer, uint8_t location, uint8_t to_mip); 
void Framebuffer_Bind(framebuffer_t* framebuffer);
void Framebuffer_UnBind();
