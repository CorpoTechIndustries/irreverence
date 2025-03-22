#pragma once

#include <public/edict.h>
#include <stddef.h>

#define EDICT_MAX_COUNT UINT16_MAX

bool ED_Init();
void ED_Close();

edict_t* ED_Create();
edict_t* ED_CreateByName(const char* name);
edict_t* ED_Get(size_t index);
void ED_Free(size_t index);

extern edict_t* g_pEdicts;
