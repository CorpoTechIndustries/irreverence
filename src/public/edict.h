#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <math/vec3.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t classname;
	uint32_t mapname;
	uint32_t model;

	float nextthink;

	vec3_t origin;

	struct edict_t* entity;
} entvars_t;

typedef struct edict_t {
	bool free;
	size_t id;
	void* data;
	entvars_t v;
} edict_t;

#ifdef __cplusplus
}
#endif
