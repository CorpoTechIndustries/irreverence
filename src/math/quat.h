#pragma once

#include <math/common.h>

#include <math/vec3.h>

#define QUAT_IDENTITY (quat_t){ 0.0f, 0.0f, 0.0f, 1.0f }

#define NEW_QUAT(x, y, z, w) (quat_t){ x, y, z, w }
#define NEW_QUATS(s) (quat_t){ s, s, s, s }

typedef union {
	vec_t v[4];

	struct {
		vec_t x, y, z, w;
	};
} quat_t;

quat_t Quat_Add(quat_t v1, quat_t v2);
quat_t Quat_Sub(quat_t v1, quat_t v2);
quat_t Quat_Mul(quat_t v1, quat_t v2);
quat_t Quat_Div(quat_t v1, quat_t v2);

void Quat_AddTo(quat_t v1, quat_t v2, quat_t* dest);
void Quat_SubTo(quat_t v1, quat_t v2, quat_t* dest);
void Quat_MulTo(quat_t v1, quat_t v2, quat_t* dest);
void Quat_DivTo(quat_t v1, quat_t v2, quat_t* dest);

void Quat_AxisAngle(float angle, vec3_t axis, quat_t* dest);

void Quat_GetAxisAngle(quat_t* quat, float* angle, vec3_t* axis);
