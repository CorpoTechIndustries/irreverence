#pragma once

#include <public/engine.h>

extern engine_functions_t g_EngineFuncs;

#define MESSAGE(fmt, ...) (g_EngineFuncs.pMessage)(fmt __VA_OPT__(,) __VA_ARGS__)
#define WARNING(fmt, ...) (g_EngineFuncs.pWarning)(fmt __VA_OPT__(,) __VA_ARGS__)
#define ERROR(fmt, ...) (g_EngineFuncs.pError)(fmt __VA_OPT__(,) __VA_ARGS__)
#define FATAL(fmt, ...) (g_EngineFuncs.pFatal)(fmt __VA_OPT__(,) __VA_ARGS__)

#define CREATE_ENTITY() (g_EngineFuncs.pEdictCreate)()
#define CREATE_ENTITY_FROM_NAME(name) (g_EngineFuncs.pEdictCreateName)(name)
#define GET_ENTITY(id) (g_EngineFuncs.pEdictGet)(id)
#define FREE_ENTITY(id) (g_EngineFuncs.pEdictFree)(id)

#define REGISTER_CVAR(cvar) (g_EngineFuncs.pRegisterCVar)(cvar)
#define GET_CVAR(name) (g_EngineFuncs.pGetCVar)(cvar)
