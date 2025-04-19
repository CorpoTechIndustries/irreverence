#include <engine/edict.h>
#include <engine/engine.h>
#include <platform/memory.h>
#include <engine/log.h>
#include <util/buffer.h>
#include <engine/server.h>
#include <engine/net_messages.h>

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

	char className[512];

	static char data[1024];

	g_GameExports.pGetEntityClass(ed, className, sizeof(className));

	buffer_t buffer = Buffer_FromMemory(data, sizeof(data));

	Buffer_WriteUInt32(&buffer, ed->id);
	Buffer_WriteString(&buffer, className, strlen(className) + 1);

	SV_NetBroadcast(&sv, SVC_EDICT_CREATE, buffer.data, buffer.pos, true);

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

	static char data[64];

	buffer_t buffer = Buffer_FromMemory(data, sizeof(data));

	Buffer_WriteUInt32(&buffer, index);

	SV_NetBroadcast(&sv, SVC_EDICT_DESTROY, buffer.data, buffer.size, true);
}
