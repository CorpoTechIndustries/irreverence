#include <engine/edict.h>
#include <engine/engine.h>
#include <platform/memory.h>
#include <engine/log.h>

edict_t* g_pEdicts = NULL;

bool ED_Init()
{
	g_pEdicts = Sys_Calloc(sizeof(edict_t) * EDICT_MAX_COUNT);

	if (!g_pEdicts) {
		return false;
	}

	for (size_t i = 0; i < EDICT_MAX_COUNT; i++) {
		g_pEdicts[i].id = i;
		g_pEdicts[i].free = true;
	}

	return true;
}

void ED_Close()
{
	if (!g_pEdicts) {
		return;
	}

	Sys_Free(g_pEdicts);
}

edict_t* ED_Create()
{
	for (size_t i = 0; i < EDICT_MAX_COUNT; i++) {
		edict_t* e = g_pEdicts + i;

		if (!e->free) {
			continue;
		}

		e->free = false;
		e->v.entity = e;
		return e;
	}

	return NULL;
}

edict_t* ED_CreateByName(const char* name)
{
	if (!g_GameDLL) {
		return NULL;
	}

	void(*map_fn)(edict_t* edict) = Sys_GetProcAddress(g_GameDLL, name);

	if (!map_fn) {
		return NULL;
	}

	edict_t* ed = ED_Create();

	if (!ed) {
		return NULL;
	}

	map_fn(ed);

	return ed;
}

edict_t* ED_Get(size_t index)
{
	if (index >= UINT16_MAX) {
		return NULL;
	}

	edict_t* e = g_pEdicts + index;

	if (e->free) {
		return NULL;
	}

	return e;
}

void ED_Free(size_t index)
{
	if (index >= UINT16_MAX) {
		return;
	}

	edict_t* e = g_pEdicts + index;

	if (e->free) {
		return;
	}

	g_GameExports.pFree(e);

	e->free = true;
}
