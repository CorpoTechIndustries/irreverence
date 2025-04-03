#include <engine/renderer/material.h>

#include <engine/renderer/texture.h>
#include <engine/renderer/renderer.h>
#include <engine/log.h>

#include <util/bit.h>

#include <platform/memory.h>

#include <string.h>

#define FM_BIT_COUNT (sizeof(uint8_t) * 8)

static uint8_t s_pFreeMaterials[MAX_MATERIALS / FM_BIT_COUNT] = { 0 }; // Literally only for generation of id which would be used for render keys.

bool Material_Init(material_t* material, texture_t** textures, uint8_t texture_count, material_flags_t flags)
{
	uint32_t id = UINT32_MAX;
	for (uint32_t i = 0; i < MAX_MATERIALS; i++) {
		if (BIT_ISSET(s_pFreeMaterials[i / FM_BIT_COUNT], i % FM_BIT_COUNT)) continue;
		
		id = i;
		break;
	}

	if (id == UINT32_MAX) {
		LOG_ERROR("Couldn't init Material due to reaching limit");
		return false;
	}

	material->id = id;

	BIT_SETON(s_pFreeMaterials[id / FM_BIT_COUNT], id % FM_BIT_COUNT);

	material->flags = flags;
	
	Sys_MemZero(material->textures, sizeof(material->textures));
	for (uint8_t i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
		texture_t* texture = textures[i];
		material->textures[i] = (texture == NULL) ? R_GetWhiteTexture() : texture;
	}

	return true;
}

void Material_Destroy(material_t* material)
{
	BIT_SETOFF(s_pFreeMaterials[material->id / FM_BIT_COUNT], material->id % FM_BIT_COUNT);
}

void Material_Bind(material_t* material)
{
	for (uint8_t i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
		texture_t* texture = material->textures[i];
		if (texture) {
			Texture_Bind(texture, i);
		}
	}
}
