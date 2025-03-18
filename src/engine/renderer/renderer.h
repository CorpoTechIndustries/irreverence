#pragma once

#include <engine/renderer/mesh.h>
#include <math/vec2.h>
#include <math/vec3.h>

#include <stdbool.h>

typedef struct Texture texture_t;

typedef uint32_t gapi_enum_t;

typedef enum {
	UNIFORM_LOCATION_GLOBAL,
	UNIFORM_LOCATION_POINTLIGHTS,
	UNIFORM_LOCATION_SPOTLIGHTS,
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

ivec2_t R_GetWindowSize();

texture_t* R_GetMissingTexture();
texture_t* R_GetWhiteTexture();
texture_t* R_GetBlackTexture();

mesh_t* R_GetCubeMesh();

extern view_t g_View;
