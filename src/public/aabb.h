#pragma once

#include <math/vec3.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	vec3_t min, max;
} aabb_t;

#ifdef __cplusplus
}
#endif
