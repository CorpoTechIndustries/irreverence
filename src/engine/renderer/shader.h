#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MAX_SHADER_NAME 64

typedef struct {
	uint32_t programId;
	char name[MAX_SHADER_NAME];
} shader_t;

bool Shader_Init(shader_t* shader, const char* name, const char* vertex_path, const char* pixel_path);
void Shader_Destroy(shader_t* shader);

void Shader_Bind(shader_t* shader);