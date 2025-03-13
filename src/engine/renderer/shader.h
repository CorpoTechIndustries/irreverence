#pragma once

#include <math/vec3.h>

#include <stdint.h>
#include <stdbool.h>

#define MAX_SHADER_NAME_LENGTH 64

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
void Shader_Dispatch(shader_t* shader, vec3_t groups, uint32_t memory_barrier);