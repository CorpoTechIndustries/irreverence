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

void Light_Init();
void Light_Destroy();

void Light_Update();

uint32_t Light_AddPointlight(const pointlight_data_t* light);
void Light_RemovePointlight(uint32_t id);

void Light_SetPLightPosition(uint32_t id, vec3_t position);
void Light_SetPLightBrightness(uint32_t id, float brightness);
void Light_SetPLightColor(uint32_t id, vec3_t color);
void Light_SetPLightRadius(uint32_t id, float radius);

uint32_t Light_AddSpotlight(const spotlight_data_t* light);
void Light_RemoveSpotlight(uint32_t id);

void Light_SetSLightPosition(uint32_t id, vec3_t position);
void Light_SetSLightCutoff(uint32_t id, float cutoff);
void Light_SetSLightDirection(uint32_t id, vec3_t direction);
void Light_SetSLightOuterCutoff(uint32_t id, float outer_cutoff);
void Light_SetSLightColor(uint32_t id, vec3_t color);
void Light_SetSLightBrightness(uint32_t id, float brightness);
