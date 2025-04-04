#include <math/quat.h>
#include <math/mat4.h>

#include <cglm/cglm.h>

#define GLM_MAT float(*)[4]

quat_t Quat_Add(quat_t q1, quat_t q2)
{
	quat_t v3;
	glm_quat_add(q1.v, q2.v, (&v3)->v);

	return v3;
}

quat_t Quat_Sub(quat_t q1, quat_t q2)
{
	quat_t v3;
	glm_quat_sub(q1.v, q2.v, (&v3)->v);
	
	return v3;
}

quat_t Quat_Mul(quat_t q1, quat_t q2)
{
	quat_t v3;
	glm_quat_mul(q1.v, q2.v, (&v3)->v);

	return v3;
}

quat_t Quat_Div(quat_t q1, quat_t q2)
{
	quat_t q2_inv = NEW_QUAT(1.0f / q2.x, 1.0f / q2.y, 1.0f / q2.z, 1.0f / q2.w);
	
	quat_t v3;
	glm_quat_mul(q1.v, q2_inv.v, (&v3)->v);

	return v3;
}

void Quat_AddTo(quat_t q1, quat_t q2, quat_t* dest)
{
	glm_quat_add(q1.v, q2.v, dest->v);
}

void Quat_SubTo(quat_t q1, quat_t q2, quat_t* dest)
{
	glm_quat_sub(q1.v, q2.v, dest->v);
}

void Quat_MulTo(quat_t q1, quat_t q2, quat_t* dest)
{
	glm_quat_mul(q1.v, q2.v, dest->v);
}

void Quat_DivTo(quat_t q1, quat_t q2, quat_t* dest)
{
	quat_t q2_inv = NEW_QUAT(1.0f / q2.x, 1.0f / q2.y, 1.0f / q2.z, 1.0f / q2.w);
	
	glm_quat_mul(q1.v, q2_inv.v, dest->v);
}

void Quat_Normalize(quat_t quat, quat_t* dest)
{
	glm_quat_normalize_to(quat.v, dest->v);
}

void Quat_AxisAngle(float angle, vec3_t axis, quat_t* dest)
{
	glm_quatv(dest->v, glm_rad(angle), axis.v);
}

void Quat_SLerp(quat_t start, quat_t end, float t, quat_t* dest)
{
	glm_quat_slerp(start.v, end.v, t, dest->v);	
}

void Quat_ToMat4(quat_t quat, mat4_t* dest)
{
	glm_quat_mat4(quat.v, (GLM_MAT)dest);
}

void Quat_GetAxisAngle(quat_t quat, float* angle, vec3_t* axis)
{
	glm_quat_axis(quat.v, axis->v);
	*angle = glm_deg(glm_quat_angle(quat.v));
}

void DualQuat_MulTo(const dualquat_t* dq1, const dualquat_t* dq2, dualquat_t* dest)
{
	Quat_MulTo(dq1->real, dq2->real, &dest->real);

	quat_t t1;
	quat_t t2;
	Quat_MulTo(dq1->real, dq2->dual, &t1);
	Quat_MulTo(dq1->dual, dq2->real, &t2);

	dest->dual = (quat_t) {
		.x = t1.x + t2.x,
		.y = t1.y + t2.y,
		.z = t1.z + t2.z,
		.w = t1.w + t2.w,
	};
}
