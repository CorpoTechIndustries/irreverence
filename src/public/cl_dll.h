#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLIENT_INTERFACE_VERSION 1

typedef struct {
	void(*pMessage)(const char* fmt, ...);
	void(*pWarning)(const char* fmt, ...);
	void(*pError)(const char* fmt, ...);
	void(*pFatal)(const char* fmt, ...);

} client_functions_t;

typedef struct {
	void(*pInit)();
	void(*pClose)();
} client_exports_t;

typedef bool(*client_init_fn_t)(client_functions_t* funcs, client_exports_t* exports, int version);


#ifdef __cplusplus
}
#endif
