#pragma once

#include <math/common.h>
#include <math/vec3.h>

#include <stdalign.h>

#ifdef __cplusplus
extern "C" {
#endif

#define QUAT_IDENTITY (quat_t){ 0.0f, 0.0f, 0.0f, 1.0f }
#define DUALQUAT_IDENTITY (dualquat_t) { QUAT_IDENTITY, NEW_QUATS(0.0f) }

#define NEW_QUAT(x, y, z, w) (quat_t){ x, y, z, w }
#define NEW_QUATS(s) (quat_t){ s, s, s, s }

typedef union mat4 mat4_t;

typedef union quat {
	alignas(16) vec_t v[4];

	struct {
		vec_t x, y, z, w;
	};
} quat_t;

typedef struct dualquat {
	alignas(16) quat_t real; // Rotation
	alignas(16) quat_t dual; // Translation
} dualquat_t;

quat_t Quat_Add(quat_t q1, quat_t q2);
quat_t Quat_Sub(quat_t q1, quat_t q2);
quat_t Quat_Mul(quat_t q1, quat_t q2);
quat_t Quat_Div(quat_t q1, quat_t q2);

void Quat_AddTo(quat_t q1, quat_t q2, quat_t* dest);
void Quat_SubTo(quat_t q1, quat_t q2, quat_t* dest);
void Quat_MulTo(quat_t q1, quat_t q2, quat_t* dest);
void Quat_DivTo(quat_t q1, quat_t q2, quat_t* dest);

void Quat_Normalize(quat_t quat, quat_t* dest);
void Quat_AxisAngle(float angle, vec3_t axis, quat_t* dest);
void Quat_SLerp(quat_t start, quat_t end, float t, quat_t* dest);

void Quat_ToMat4(quat_t quat, mat4_t* dest);
void Quat_GetAxisAngle(quat_t quat, float* angle, vec3_t* axis);

void DualQuat_MulTo(const dualquat_t* dq1, const dualquat_t* dq2, dualquat_t* dest);

#ifdef __cplusplus
}
#endif
