#pragma once

#include <math/common.h>

#include <stdalign.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VEC4_ZERO (vec4_t){ 0.0f, 0.0f, 0.0f, 0.0f }
#define IVEC4_ZERO (ivec4_t){ 0, 0, 0, 0 }

#define NEW_VEC4(x, y, z, w) (vec4_t){ x, y, z, w }
#define NEW_VEC4S(s) (vec4_t){ s, s, s, s }

#define NEW_IVEC4(x, y, z, w) (ivec4_t){ x, y, z, w }
#define NEW_IVEC4S(s) (ivec4_t){ s, s, s, s }

typedef union vec4 {
	alignas(16) vec_t v[4];

	struct {
		vec_t x, y, z, w;
	};
} vec4_t;

typedef union ivec4 {
	alignas(16) ivec_t v[4];

	struct {
		ivec_t x, y, z, w;
	};
} ivec4_t;

vec4_t Vec4_Add(vec4_t v1, vec4_t v2);
vec4_t Vec4_Adds(vec4_t v1, vec_t scale);
vec4_t Vec4_Sub(vec4_t v1, vec4_t v2);
vec4_t Vec4_Subs(vec4_t v1, vec_t scale);
vec4_t Vec4_Mul(vec4_t v1, vec4_t v2);
vec4_t Vec4_Muls(vec4_t v1, vec_t scale);
vec4_t Vec4_Div(vec4_t v1, vec4_t v2);
vec4_t Vec4_Divs(vec4_t v1, vec_t scale);

void Vec4_AddTo(vec4_t v1, vec4_t v2, vec4_t* dest);
void Vec4_AddsTo(vec4_t v1, vec_t scale, vec4_t* dest);
void Vec4_SubTo(vec4_t v1, vec4_t v2, vec4_t* dest);
void Vec4_SubsTo(vec4_t v1, vec_t scale, vec4_t* dest);
void Vec4_MulTo(vec4_t v1, vec4_t v2, vec4_t* dest);
void Vec4_MulsTo(vec4_t v, vec_t scale, vec4_t* dest);
void Vec4_DivTo(vec4_t v1, vec4_t v2, vec4_t* dest);
void Vec4_DivsTo(vec4_t v1, vec_t scale, vec4_t* dest);

ivec4_t IVec4_Add(ivec4_t v1, ivec4_t v2);
ivec4_t IVec4_Adds(ivec4_t v1, ivec_t scale);
ivec4_t IVec4_Sub(ivec4_t v1, ivec4_t v2);
ivec4_t IVec4_Subs(ivec4_t v1, ivec_t scale);
ivec4_t IVec4_Mul(ivec4_t v1, ivec4_t v2);
ivec4_t IVec4_Muls(ivec4_t v1, ivec_t scale);
ivec4_t IVec4_Div(ivec4_t v1, ivec4_t v2);
ivec4_t IVec4_Divs(ivec4_t v1, ivec_t scale);

void IVec4_AddTo(ivec4_t v1, ivec4_t v2, ivec4_t* dest);
void IVec4_AddsTo(ivec4_t v1, ivec_t scale, ivec4_t* dest);
void IVec4_SubTo(ivec4_t v1, ivec4_t v2, ivec4_t* dest);
void IVec4_SubsTo(ivec4_t v1, ivec_t scale, ivec4_t* dest);
void IVec4_MulTo(ivec4_t v1, ivec4_t v2, ivec4_t* dest);
void IVec4_MulsTo(ivec4_t v, ivec_t scale, ivec4_t* dest);
void IVec4_DivTo(ivec4_t v1, ivec4_t v2, ivec4_t* dest);
void IVec4_DivsTo(ivec4_t v1, ivec_t scale, ivec4_t* dest);

#ifdef __cplusplus
}
#endif
