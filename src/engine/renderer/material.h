#pragma once

#include <engine/renderer/texture.h>

#include <stdbool.h>

#define MAX_MATERIALS UINT32_MAX

#define MAX_MATERIAL_TEXTURES 1

typedef enum {
	MATERIAL_TEXTURE_DIFFUSE
} material_texture_t;

typedef enum {
	MATERIAL_FLAG_TRANSPARENT,
	MATERIAL_FLAG_FULLBRIGHT
} material_flags_t;

typedef struct {
	uint32_t id;
	uint32_t flags;
	texture_t* textures[MAX_MATERIAL_TEXTURES]; // Would use default textures if not existant
} material_t;

bool Material_Init(material_t* material, texture_t** textures, uint8_t texture_count, material_flags_t flags);
void Material_Destroy(material_t* material);

void Material_Bind(material_t* material);
