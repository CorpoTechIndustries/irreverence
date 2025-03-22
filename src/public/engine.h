#pragma once

#include <public/edict.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ENGINE_INTERFACE_VERSION 1

// Engine exported functions
typedef struct {
	void(*pMessage)(const char* fmt, ...);
	void(*pWarning)(const char* fmt, ...);
	void(*pError)(const char* fmt, ...);
	void(*pFatal)(const char* fmt, ...);

	edict_t*(*pEdictCreate)();
	edict_t*(*pEdictCreateName)(const char* className);
	edict_t*(*pEdictGet)(size_t index);
	void(*pEdictFree)(size_t index);
} engine_functions_t;

// Functions exported by the game dll
typedef struct {
	void(*pGameInit)();
	void(*pGameClose)();

	bool(*pSpawn)(edict_t* ent);
	void(*pFree)(edict_t* ent);
	void(*pThink)(edict_t* ent);
	void(*pUse)(edict_t* user, edict_t* used);
	void(*pTouch)(edict_t* touched, edict_t* ent);
} game_exports_t;

#ifdef __cplusplus
}
#endif
