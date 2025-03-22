#pragma once

#include <math/common.h>

#include <math/vec3.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAT4_IDENTITY (mat4_t){ 1.0f, 0.0f, 0.0f, 0.0f,	\
								0.0f, 1.0f, 0.0f, 0.0f, \
								0.0f, 0.0f, 1.0f, 0.0f, \
								0.0f, 0.0f, 0.0f, 1.0f }

#define MAT4_ZERO (mat4_t){ 0.0f, 0.0f, 0.0f, 0.0f,	\
							0.0f, 0.0f, 0.0f, 0.0f, \
							0.0f, 0.0f, 0.0f, 0.0f, \
							0.0f, 0.0f, 0.0f, 0.0f }

typedef union {
	vec_t v[16];

	struct {
		vec_t m0,  m1,  m2,  m3;
		vec_t m4,  m5,  m6,  m7;
		vec_t m8,  m9,  m10, m11;
		vec_t m12, m13, m14, m15;
	};
} mat4_t;

void Mat4_Mul(const mat4_t* m1, const mat4_t* m2, mat4_t* dest);
void Mat4_Inv(const mat4_t* mat, mat4_t* dest);

void Mat4_Translate(mat4_t* mat, vec3_t offset);
void Mat4_Scale(mat4_t* mat, vec3_t scale);
void Mat4_Rotate(mat4_t* mat, float angle, vec3_t axis);

void Mat4_Ortho(float left, float right, float bottom, float top, float nearZ, float farZ, mat4_t* dest);
void Mat4_Perspective(float fov, float aspect, float nearZ, float farZ, mat4_t* dest);
void Mat4_LookAt(vec3_t eye, vec3_t center, vec3_t up, mat4_t* dest);

#ifdef __cplusplus
}
#endif
