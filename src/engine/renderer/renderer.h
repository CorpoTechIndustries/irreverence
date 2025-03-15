#pragma once

#include <engine/renderer/texture.h>
#include <engine/renderer/mesh.h>
#include <math/vec2.h>
#include <math/vec3.h>

#include <stdbool.h>

typedef struct {
	vec3_t position;
	vec3_t direction;
	vec3_t up;

	float fov;
	float nearZ;
	float farZ;
} view_t;

bool R_Init();
void R_Destroy();
void R_WindowUpdate(int width, int height);
void R_DebugMoveUpdate();

void R_Present();

ivec2_t R_GetWindowSize();

texture_t* R_GetMissingTexture();
texture_t* R_GetWhiteTexture();
texture_t* R_GetBlackTexture();

mesh_t* R_GetCubeMesh();

extern view_t g_View;
