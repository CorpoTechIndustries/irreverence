#pragma once

#include <math/common.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VEC2_ZERO (vec2_t){ 0.0f, 0.0f }
#define IVEC2_ZERO (ivec2_t){ 0, 0 }

#define NEW_VEC2(x, y) (vec2_t){ x, y }
#define NEW_VEC2S(s) (vec2_t){ s, s }

#define NEW_IVEC2(x, y) (ivec2_t){ x, y }
#define NEW_IVEC2S(s) (ivec2_t){ s, s }

typedef union vec2 {
	vec_t v[2];

	struct {
		vec_t x, y;
	};

	struct {
		vec_t r, g;
	};
} vec2_t;

typedef union ivec2 {
	ivec_t v[2];

	struct {
		ivec_t x, y;
	};

	struct {
		ivec_t r, g;
	};
} ivec2_t;

vec2_t Vec2_Add(vec2_t v1, vec2_t v2);
vec2_t Vec2_Adds(vec2_t v1, vec_t scale);
vec2_t Vec2_Sub(vec2_t v1, vec2_t v2);
vec2_t Vec2_Subs(vec2_t v1, vec_t scale);
vec2_t Vec2_Mul(vec2_t v1, vec2_t v2);
vec2_t Vec2_Muls(vec2_t v1, vec_t scale);
vec2_t Vec2_Div(vec2_t v1, vec2_t v2);
vec2_t Vec2_Divs(vec2_t v1, vec_t scale);

void Vec2_AddTo(vec2_t v1, vec2_t v2, vec2_t* dest);
void Vec2_AddsTo(vec2_t v1, vec_t scale, vec2_t* dest);
void Vec2_SubTo(vec2_t v1, vec2_t v2, vec2_t* dest);
void Vec2_SubsTo(vec2_t v1, vec_t scale, vec2_t* dest);
void Vec2_MulTo(vec2_t v1, vec2_t v2, vec2_t* dest);
void Vec2_MulsTo(vec2_t v1, vec_t scale, vec2_t* dest);
void Vec2_DivTo(vec2_t v1, vec2_t v2, vec2_t* dest);
void Vec2_DivsTo(vec2_t v1, vec_t scale, vec2_t* dest);

void Vec2_Normalize(vec2_t vec, vec2_t* dest);
float Vec2_Cross(vec2_t v1, vec2_t v2);
float Vec2_Dot(vec2_t v1, vec2_t v2);

ivec2_t IVec2_Add(ivec2_t v1, ivec2_t v2);
ivec2_t IVec2_Adds(ivec2_t v1, ivec_t scale);
ivec2_t IVec2_Sub(ivec2_t v1, ivec2_t v2);
ivec2_t IVec2_Subs(ivec2_t v1, ivec_t scale);
ivec2_t IVec2_Mul(ivec2_t v1, ivec2_t v2);
ivec2_t IVec2_Muls(ivec2_t v1, ivec_t scale);
ivec2_t IVec2_Div(ivec2_t v1, ivec2_t v2);
ivec2_t IVec2_Divs(ivec2_t v1, ivec_t scale);

void IVec2_AddTo(ivec2_t v1, ivec2_t v2, ivec2_t* dest);
void IVec2_AddsTo(ivec2_t v1, ivec_t scale, ivec2_t* dest);
void IVec2_SubTo(ivec2_t v1, ivec2_t v2, ivec2_t* dest);
void IVec2_SubsTo(ivec2_t v1, ivec_t scale, ivec2_t* dest);
void IVec2_MulTo(ivec2_t v1, ivec2_t v2, ivec2_t* dest);
void IVec2_MulsTo(ivec2_t v1, ivec_t scale, ivec2_t* dest);
void IVec2_DivTo(ivec2_t v1, ivec2_t v2, ivec2_t* dest);
void IVec2_DivsTo(ivec2_t v1, ivec_t scale, ivec2_t* dest);

int IVec2_Dot(ivec2_t v1, ivec2_t v2);

#ifdef __cplusplus
}
#endif
