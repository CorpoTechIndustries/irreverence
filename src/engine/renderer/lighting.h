#pragma once

#include <math/vec3.h>

#include <stdbool.h>
#include <stdint.h>

#define MAX_POINTLIGHTS 512
#define MAX_SPOTLIGHTS 512

typedef struct {
	vec3_t position;
	float brightness;
	vec3_t color;
	float radius;
} pointlight_data_t;

typedef struct {
	vec3_t position;
	float cutoff;
	vec3_t direction;
	float outerCutoff;
	vec3_t color;
	float brightness;
} spotlight_data_t;

void Lighting_Init();
void Lighting_Destroy();

void Lighting_Update();

uint32_t Lighting_AddPointlight(const pointlight_data_t* light);
void Lighting_RemovePointlight(uint32_t id);

void Lighting_SetPLightPosition(uint32_t id, vec3_t position);
void Lighting_SetPLightBrightness(uint32_t id, float brightness);
void Lighting_SetPLightColor(uint32_t id, vec3_t color);
void Lighting_SetPLightRadius(uint32_t id, float radius);

uint32_t Lighting_AddSpotlight(const spotlight_data_t* light);
void Lighting_RemoveSpotlight(uint32_t id);

void Lighting_SetSLightPosition(uint32_t id, vec3_t position);
void Lighting_SetSLightCutoff(uint32_t id, float cutoff);
void Lighting_SetSLightDirection(uint32_t id, vec3_t direction);
void Lighting_SetSLightOuterCutoff(uint32_t id, float outer_cutoff);
void Lighting_SetSLightColor(uint32_t id, vec3_t color);
void Lighting_SetSLightBrightness(uint32_t id, float brightness);
