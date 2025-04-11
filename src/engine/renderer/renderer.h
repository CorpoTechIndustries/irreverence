#pragma once

#include <engine/renderer/mesh.h>
#include <engine/renderer/animation.h>
#include <engine/renderer/material.h>
#include <math/vec2.h>
#include <math/vec3.h>

#include <stdbool.h>

typedef struct texture texture_t;

typedef uint32_t gapi_enum_t;

typedef enum {
	UNIFORM_LOCATION_COMMON,
	UNIFORM_LOCATION_POINTLIGHTS,
	UNIFORM_LOCATION_SPOTLIGHTS,
	UNIFORM_LOCATION_ANIMATIONS
} uniform_location_t;

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

void R_UpdateAnimationBuffer(animator_t* animator);

ivec2_t R_GetWindowSize();

texture_t* R_GetMissingTexture();
texture_t* R_GetWhiteTexture();
texture_t* R_GetBlackTexture();

material_t* R_GetWhiteMaterial();

mesh_t* R_GetCubeMesh();

extern view_t g_View;
