#pragma once

#include <math/common.h>

#define VEC2_ZERO (vec2_t){ 0.0f, 0.0f }
#define IVEC2_ZERO (ivec2_t){ 0, 0 }

#define NEW_VEC2(x, y) (vec2_t){ x, y }
#define NEW_IVEC2(x, y) (ivec2_t){ x, y }

typedef union {
	vec_t v[2];

	struct {
		vec_t x, y;
	};

	struct {
		vec_t r, g;
	};
} vec2_t;

typedef union {
	ivec_t v[2];

	struct {
		ivec_t x, y;
	};

	struct {
		ivec_t r, g;
	};
} ivec2_t;

void Vec2_Add(vec2_t v1, vec2_t v2, vec2_t* dest);
void Vec2_Sub(vec2_t v1, vec2_t v2, vec2_t* dest);
void Vec2_Mul(vec2_t v1, vec2_t v2, vec2_t* dest);
void Vec2_Div(vec2_t v1, vec2_t v2, vec2_t* dest);

void Vec2_Normalize(vec2_t vec, vec2_t* dest);
float Vec2_Cross(vec2_t v1, vec2_t v2);
float Vec2_Dot(vec2_t v1, vec2_t v2);

void IVec2_Add(ivec2_t v1, ivec2_t v2, ivec2_t* dest);
void IVec2_Sub(ivec2_t v1, ivec2_t v2, ivec2_t* dest);
void IVec2_Mul(ivec2_t v1, ivec2_t v2, ivec2_t* dest);
void IVec2_Div(ivec2_t v1, ivec2_t v2, ivec2_t* dest);

int IVec2_Dot(ivec2_t v1, ivec2_t v2);
