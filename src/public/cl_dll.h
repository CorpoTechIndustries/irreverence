#pragma once

#include <stdbool.h>
#include <public/cvar.h>
#include <public/edict.h>

#include <math/vec3.h>
#include <math/vec4.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLIENT_INTERFACE_VERSION 1

typedef edict_t client_entity_t;

typedef struct {
	void(*pMessage)(const char* fmt, ...);
	void(*pWarning)(const char* fmt, ...);
	void(*pError)(const char* fmt, ...);
	void(*pFatal)(const char* fmt, ...);

	void(*pRegisterCVar)(cvar_t* cvar);
	cvar_t*(*pGetCVar)(const char* name);

	void(*pDebugDrawCube)(vec3_t origin, vec3_t size, vec4_t color);
} client_functions_t;

typedef struct {
	void(*pInit)();
	void(*pClose)();

	bool(*pEntityCreate)(client_entity_t* entity, const char* className);
	void(*pEntityDestroy)(client_entity_t* entity);
	void(*pEntityThink)(client_entity_t* entity);
	void(*pEntityRender)(client_entity_t* entity);

	bool(*pRenderPreDepth)();
	void(*pRenderPostDepth)();
	bool(*pRenderPreShadows)();
	void(*pRenderPostShadows)();
	bool(*pRenderPreOpaque)();
	void(*pRenderPostOpaque)();
	bool(*pRenderPreTransparent)();
	void(*pRenderPostTransparent)();
	bool(*pRenderPreDecals)();
	void(*pRenderPostDecals)();
	bool(*pRenderPreOverlay)();
	void(*pRenderPostOverlay)();
} client_exports_t;

typedef bool(*client_init_fn_t)(client_functions_t* funcs, client_exports_t* exports, int version);


#ifdef __cplusplus
}
#endif
