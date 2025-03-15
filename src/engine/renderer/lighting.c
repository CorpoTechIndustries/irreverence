#include <engine/renderer/lighting.h>

#include <engine/renderer/renderer.h>
#include <engine/renderer/uniform.h>
#include <engine/log.h>

#include <util/array.h>

#include <platform/memory.h>

#include <math.h>

typedef struct {
	pointlight_data_t data;
	bool ready;
	bool outdated;
} pointlight_t;

typedef struct {
	spotlight_data_t data;
	bool ready;
	bool outdated;
} spotlight_t;

static uniform_t s_PointlightUniform;
static uniform_t s_SpotlightUniform;

static pointlight_t* s_pPointlights = NULL; 
static spotlight_t* s_pSpotlights = NULL; 

void Lighting_Init()
{
	Uniform_Init(&s_PointlightUniform, UNIFORM_LOCATION_POINTLIGHTS, NULL, sizeof(pointlight_data_t) * MAX_POINTLIGHTS + sizeof(uint32_t));
	Uniform_Init(&s_SpotlightUniform, UNIFORM_LOCATION_SPOTLIGHTS, NULL, sizeof(spotlight_data_t) * MAX_SPOTLIGHTS + sizeof(uint32_t));

	s_pPointlights = Array_CreateSize(pointlight_t, MAX_POINTLIGHTS);
	Sys_MemZero(s_pPointlights, sizeof(pointlight_t) * MAX_POINTLIGHTS);
	
	s_pSpotlights = Array_CreateSize(spotlight_t, MAX_SPOTLIGHTS);
	Sys_MemZero(s_pSpotlights, sizeof(spotlight_t) * MAX_SPOTLIGHTS);
}

void Lighting_Destroy()
{
	if (s_pPointlights) Array_Destroy(s_pPointlights);
	if (s_pSpotlights) Array_Destroy(s_pSpotlights);
}

void Lighting_Update()
{
	uint32_t pointlightCount = 0;
	for (uint32_t i = 0; i < MAX_POINTLIGHTS; i++) {
		pointlight_t* light = &s_pPointlights[i];

		if (!light->ready) continue;
		pointlightCount = i + 1;

		if (!light->outdated) continue;
		light->outdated = false;

		Uniform_Update(&s_PointlightUniform, &light->data, sizeof(pointlight_data_t), sizeof(pointlight_data_t) * i);
	}
	Uniform_Update(&s_PointlightUniform, &pointlightCount, sizeof(uint32_t), sizeof(pointlight_data_t) * MAX_POINTLIGHTS);

	uint32_t spotlightCount = 0;
	for (uint32_t i = 0; i < MAX_SPOTLIGHTS; i++) {
		spotlight_t* light = &s_pSpotlights[i];

		if (!light->ready || !light->outdated) continue;
		light->outdated = true;

		Uniform_Update(&s_SpotlightUniform, &light->data, sizeof(spotlight_data_t), sizeof(spotlight_data_t) * i);
		spotlightCount = i + 1;
	}
	Uniform_Update(&s_SpotlightUniform, &spotlightCount, sizeof(uint32_t), sizeof(spotlight_data_t) * MAX_SPOTLIGHTS);
}

uint32_t Lighting_AddPointlight(const pointlight_data_t* light)
{
	uint32_t id = UINT32_MAX;
	for (uint32_t i = 0; i < MAX_POINTLIGHTS; i++) {
		pointlight_t* plight = &s_pPointlights[i];
		
		if (plight->ready) continue;

		id = i;
		break;
	}

	if (id == UINT32_MAX) {
		LOG_ERROR("Couldn't add Pointlight due to reaching limit");
		return UINT32_MAX;
	}

	pointlight_t* plight = &s_pPointlights[id];
	plight->data = *light;
	plight->outdated = true;
	plight->ready = true;

	return id;
}

void Lighting_RemovePointlight(uint32_t id)
{
	if (id >= MAX_POINTLIGHTS || !s_pPointlights[id].ready) return;

	Sys_MemZero(&s_pPointlights[id], sizeof(pointlight_t));

	pointlight_data_t emptylight = {
		.position = NEW_VEC3S(0.0f),
		.color = NEW_VEC3S(0.0f),
		.brightness = 0.0f,
		.radius = 0.0f
	};

	Uniform_Update(&s_PointlightUniform, &emptylight, sizeof(pointlight_data_t), sizeof(pointlight_data_t) * id);
}

void Lighting_SetPLightPosition(uint32_t id, vec3_t position)
{
	if (id >= MAX_POINTLIGHTS) return;

	pointlight_t* light = &s_pPointlights[id];
	if (!light->ready) return;

	light->data.position = position;
	light->outdated = true;
}

void Lighting_SetPLightBrightness(uint32_t id, float brightness)
{
	if (id >= MAX_POINTLIGHTS) return;

	pointlight_t* light = &s_pPointlights[id];
	if (!light->ready) return;

	light->data.brightness = brightness;
	light->outdated = true;
}

void Lighting_SetPLightColor(uint32_t id, vec3_t color)
{
	if (id >= MAX_POINTLIGHTS) return;

	pointlight_t* light = &s_pPointlights[id];
	if (!light->ready) return;

	light->data.color = color;
	light->outdated = true;
}

void Lighting_SetPLightRadius(uint32_t id, float radius)
{
	if (id >= MAX_POINTLIGHTS) return;

	pointlight_t* light = &s_pPointlights[id];
	if (!light->ready) return;

	light->data.radius = radius;
	light->outdated = true;
}

uint32_t Lighting_AddSpotlight(const spotlight_data_t* light)
{
	uint32_t id = UINT32_MAX;
	for (uint32_t i = 0; i < MAX_SPOTLIGHTS; i++) {
		spotlight_t* slight = &s_pSpotlights[i];
		
		if (slight->ready) continue;

		id = i;
		break;
	}

	if (id == UINT32_MAX) {
		LOG_ERROR("Couldn't add Spotlight due to reaching limit");
		return UINT32_MAX;
	}

	spotlight_t* slight = &s_pSpotlights[id];
	slight->data = *light;
	slight->data.cutoff = cosf(Math_Rad(light->cutoff));
	slight->data.outerCutoff = cosf(Math_Rad(light->outerCutoff));
	slight->outdated = true;
	slight->ready = true;

	return id;
}

void Lighting_RemoveSpotlight(uint32_t id)
{
	if (id >= MAX_SPOTLIGHTS || !s_pSpotlights[id].ready) return;

	Sys_MemZero(&s_pSpotlights[id], sizeof(spotlight_t));

	spotlight_data_t emptylight = {
		.position = NEW_VEC3S(0.0f),
		.direction = NEW_VEC3S(0.0f),
		.color = NEW_VEC3S(0.0f),
		.brightness = 0.0f,
		.cutoff = 0.0f,
		.outerCutoff = 0.0f
	};

	Uniform_Update(&s_SpotlightUniform, &emptylight, sizeof(spotlight_data_t), sizeof(spotlight_data_t) * id);
}

void Lighting_SetSLightPosition(uint32_t id, vec3_t position)
{
	if (id >= MAX_SPOTLIGHTS) return;

	spotlight_t* light = &s_pSpotlights[id];
	if (!light->ready) return;

	light->data.position = position;
	light->outdated = true;
}

void Lighting_SetSLightCutoff(uint32_t id, float cutoff)
{
	if (id >= MAX_SPOTLIGHTS) return;

	spotlight_t* light = &s_pSpotlights[id];
	if (!light->ready) return;

	light->data.cutoff = cutoff;
	light->outdated = true;
}

void Lighting_SetSLightDirection(uint32_t id, vec3_t direction)
{
	if (id >= MAX_SPOTLIGHTS) return;

	spotlight_t* light = &s_pSpotlights[id];
	if (!light->ready) return;

	light->data.direction = direction;
	light->outdated = true;
}

void Lighting_SetSLightOuterCutoff(uint32_t id, float outer_cutoff)
{
	if (id >= MAX_SPOTLIGHTS) return;

	spotlight_t* light = &s_pSpotlights[id];
	if (!light->ready) return;

	light->data.outerCutoff = outer_cutoff;
	light->outdated = true;
}

void Lighting_SetSLightColor(uint32_t id, vec3_t color)
{
	if (id >= MAX_SPOTLIGHTS) return;

	spotlight_t* light = &s_pSpotlights[id];
	if (!light->ready) return;

	light->data.color = color;
	light->outdated = true;
}

void Lighting_SetSLightBrightness(uint32_t id, float brightness)
{
	if (id >= MAX_SPOTLIGHTS) return;

	spotlight_t* light = &s_pSpotlights[id];
	if (!light->ready) return;

	light->data.brightness = brightness;
	light->outdated = true;
}
