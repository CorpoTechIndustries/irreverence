#pragma once

#include <engine/renderer/texture.h>
#include <util/bitset.h>

#define MAX_MATERIAL_NAME_LENGTH 64
#define MAX_MATERIAL_TEXTURES 1

typedef enum {
	MATERIAL_TEXTURE_DIFFUSE
} material_texture_t;

typedef enum {
	MATERIAL_FLAG_TRANSPARENT,
	MATERIAL_FLAG_FULLBRIGHT
} material_flags_t;

typedef struct {
	char name[MAX_MATERIAL_NAME_LENGTH];
	uint32_t flags;
	texture_t* textures[MAX_MATERIAL_TEXTURES]; // Would use default textures if not existant
} material_t;

void Material_Init(material_t* material, const char* name, texture_t** textures, uint8_t texture_count, material_flags_t flags);
void Material_Destroy(material_t* material);

void Material_Bind(material_t* material);