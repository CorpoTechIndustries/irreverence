#pragma once

#include <math/common.h>

typedef union {
	vec_t val[16];
	struct {
		vec_t m0,  m1,  m2,  m3;
		vec_t m4,  m5,  m6,  m7;
		vec_t m8,  m9,  m10, m11;
		vec_t m12, m13, m14, m15;
	};
} mat4_t;