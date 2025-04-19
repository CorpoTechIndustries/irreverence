#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MAX_MATERIALS UINT32_MAX

#define MAX_MATERIAL_TEXTURES 1

typedef struct texture texture_t;
typedef struct texture_array texture_array_t;

typedef enum {
	MATERIAL_TEXTURE_DIFFUSE
} material_texture_t;

typedef enum {
	MATERIAL_FLAG_TRANSPARENT,
	MATERIAL_FLAG_FULLBRIGHT,
	MATERIAL_FLAG_TEXTUREARRAY, 	// Does it use an texture array? This automatically gets set in Init
} material_flags_t;

typedef struct material {
	uint32_t id;
	uint32_t flags;
	union {
		texture_t* textures[MAX_MATERIAL_TEXTURES]; // Would use default textures if not existant
		texture_array_t* textureArray; // Uses default Texture Array 
	};
} material_t;

bool Material_InitTextures(material_t* material, texture_t** textures, uint8_t texture_count, material_flags_t flags);
bool Material_InitTextureArray(material_t* material, texture_array_t* texture_array, material_flags_t flags);
void Material_Destroy(material_t* material);

void Material_Bind(material_t* material);
