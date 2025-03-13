#include <engine/renderer/material.h>
#include <engine/renderer/renderer.h>

#include <platform/memory.h>

#include <string.h>

void Material_Init(material_t* material, const char* name, texture_t** textures, uint8_t texture_count, material_flags_t flags)
{
	strncpy(material->name, name, MAX_MATERIAL_NAME_LENGTH);
	material->flags = flags;
	
	Sys_MemZero(material->textures, sizeof(material->textures));
	for (uint8_t i = 0; i < MAX_MATERIAL_TEXTURES; i++) {
		texture_t* texture = textures[i];
		material->textures[i] = (texture == NULL) ? R_GetWhiteTexture() : texture;
	}
	
	// TODO: Store Material, or maybe instead of storing it here, store it on Asset Manager?
}

void Material_Destroy(material_t* material)
{
	
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