#pragma once

#include <math/vec2.h>
#include <math/vec3.h>
#include <math/vec4.h>

#include <stdint.h>
#include <stdbool.h>

#define MAX_SHADER_NAME_LENGTH 64

typedef uint32_t gapi_enum_t;

typedef struct {
	uint32_t id;
	char name[MAX_SHADER_NAME_LENGTH];
} shader_t;

bool Shader_InitRaster(shader_t* shader, const char* name, const char* vertex_path, const char* pixel_path);
bool Shader_InitRasterFromMemory(shader_t* shader, const char* vertex_code, const char* pixel_code);
bool Shader_InitCompute(shader_t* shader, const char* name, const char* path);
bool Shader_InitComputeFromMemory(shader_t* shader, const char* code);
void Shader_Destroy(shader_t* shader);

void Shader_Bind(shader_t* shader);
void Shader_Dispatch(shader_t* shader, ivec3_t groups, gapi_enum_t memory_barrier);

uint8_t Shader_GetLocation(shader_t* shader, const char* name);

void Shader_SetInt(shader_t* shader, uint8_t location, int32_t v);
void Shader_SetUInt(shader_t* shader, uint8_t location, uint32_t v);
void Shader_SetFloat(shader_t* shader, uint8_t location, float v);
void Shader_SetVec2(shader_t* shader, uint8_t location, vec2_t v);
void Shader_SetVec3(shader_t* shader, uint8_t location, vec3_t v);
void Shader_SetVec4(shader_t* shader, uint8_t location, vec4_t v);
void Shader_SetIVec2(shader_t* shader, uint8_t location, ivec2_t v);
void Shader_SetIVec3(shader_t* shader, uint8_t location, ivec3_t v);
void Shader_SetIVec4(shader_t* shader, uint8_t location, ivec4_t v);
