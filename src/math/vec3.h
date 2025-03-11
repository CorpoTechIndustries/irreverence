#pragma once

#include <math/common.h>

#define VEC3_ZERO (vec3_t){ 0.0f, 0.0f, 0.0f }
#define IVEC3_ZERO (ivec3_t){ 0, 0, 0 }

#define NEW_VEC3(x, y, z) (vec3_t){ x, y, z }
#define NEW_VEC3S(s) (vec3_t){ s, s, s }

#define NEW_IVEC3(x, y, z) (ivec3_t){ x, y, z }
#define NEW_IVEC3S(s) (ivec3_t){ s, s, s }

typedef union {
	vec_t v[3];

	struct {
		vec_t x, y, z;
	};

	struct {
		vec_t r, g, b;
	};
} vec3_t;

typedef union {
	ivec_t v[3];

	struct {
		ivec_t x, y, z;
	};
	
	struct {
		ivec_t r, g, b;
	};
} ivec3_t;

void Vec3_Add(vec3_t v1, vec3_t v2, vec3_t* dest);
void Vec3_Sub(vec3_t v1, vec3_t v2, vec3_t* dest);
void Vec3_Mul(vec3_t v1, vec3_t v2, vec3_t* dest);
void Vec3_Div(vec3_t v1, vec3_t v2, vec3_t* dest);

void Vec3_Normalize(vec3_t vec, vec3_t* dest);
void Vec3_Cross(vec3_t v1, vec3_t v2, vec3_t* dest);
float Vec3_Dot(vec3_t v1, vec3_t v2);

void IVec3_Add(ivec3_t v1, ivec3_t v2, ivec3_t* dest);
void IVec3_Sub(ivec3_t v1, ivec3_t v2, ivec3_t* dest);
void IVec3_Mul(ivec3_t v1, ivec3_t v2, ivec3_t* dest);
void IVec3_Div(ivec3_t v1, ivec3_t v2, ivec3_t* dest);

int IVec3_Dot(ivec3_t v1, ivec3_t v2);