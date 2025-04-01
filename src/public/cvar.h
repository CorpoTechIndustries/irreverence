#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	CVAR_INT,
	CVAR_FLOAT,
	CVAR_BOOL,
	CVAR_STRING
} cvar_type_t;

typedef struct {
	const char* name;

	cvar_type_t type;
	union {
		int as_int;
		float as_float;
		bool as_bool;
		char* as_string;
	} value;
} cvar_t;

#define MAKE_CVAR_INT(__name, __default) \
cvar_t __name = { .name = #__name, .type = CVAR_INT, .value = { .as_int = __default } }
#define MAKE_CVAR_FLOAT(__name, __default) \
cvar_t __name = { .name = #__name, .type = CVAR_FLOAT, .value = { .as_float = __default } }
#define MAKE_CVAR_BOOL(__name, __default) \
cvar_t __name = { .name = #__name, .type = CVAR_BOOL, .value = { .as_bool = __default } }
#define MAKE_CVAR_STRING(__name, __default) \
cvar_t __name = { .name = #__name, .type = CVAR_STRING, .value = { .as_string = __default } }



#ifdef __cplusplus
}
#endif
