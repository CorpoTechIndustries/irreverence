#pragma once

#include <public/cvar.h>

void CVar_Register(cvar_t* cvar);
cvar_t* CVar_Get(const char* name);

int CVar_GetInt(const char* name);
float CVar_GetFloat(const char* name);
bool CVar_GetBool(const char* name);
const char* CVar_GetString(const char* name);

void CVar_SetInt(const char* name, int value);
void CVar_SetFloat(const char* name, float value);
void CVar_SetBool(const char* name, bool value);
void CVar_SetString(const char* name, const char* value);
