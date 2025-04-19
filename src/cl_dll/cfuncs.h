#pragma once

#include <public/cl_dll.h>

extern client_functions_t g_ClientFuncs;

#define MESSAGE(fmt, ...) (g_ClientFuncs.pMessage)(fmt __VA_OPT__(,) __VA_ARGS__)
#define WARNING(fmt, ...) (g_ClientFuncs.pWarning)(fmt __VA_OPT__(,) __VA_ARGS__)
#define ERROR(fmt, ...) (g_ClientFuncs.pError)(fmt __VA_OPT__(,) __VA_ARGS__)
#define FATAL(fmt, ...) (g_ClientFuncs.pFatal)(fmt __VA_OPT__(,) __VA_ARGS__)

#define REGISTER_CVAR(cvar) (g_ClientFuncs.pRegisterCVar)(cvar)
#define GET_CVAR(name) (g_ClientFuncs.pGetCVar)(name)

#define DRAW_DEBUG_CUBE(origin, size, color) (g_ClientFuncs.pDebugDrawCube)(origin, size, color)
