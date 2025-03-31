#include <math/quat.h>

#include <cglm/cglm.h>

quat_t Quat_Add(quat_t v1, quat_t v2)
{
	quat_t v3;
	glm_quat_add(v1.v, v2.v, (&v3)->v);

	return v3;
}

quat_t Quat_Sub(quat_t v1, quat_t v2)
{
	quat_t v3;
	glm_quat_sub(v1.v, v2.v, (&v3)->v);

	return v3;
}

quat_t Quat_Mul(quat_t v1, quat_t v2)
{
	quat_t v3;
	glm_quat_mul(v1.v, v2.v, (&v3)->v);

	return v3;
}

quat_t Quat_Div(quat_t v1, quat_t v2)
{
	quat_t v2_inv = NEW_QUAT(1.0f / v2.x, 1.0f / v2.y, 1.0f / v2.z, 1.0f / v2.w);
	
	quat_t v3;
	glm_quat_mul(v1.v, v2_inv.v, (&v3)->v);

	return v3;
}

void Quat_AddTo(quat_t v1, quat_t v2, quat_t* dest)
{
	glm_quat_add(v1.v, v2.v, dest->v);
}

void Quat_SubTo(quat_t v1, quat_t v2, quat_t* dest)
{
	glm_quat_sub(v1.v, v2.v, dest->v);
}

void Quat_MulTo(quat_t v1, quat_t v2, quat_t* dest)
{
	glm_quat_mul(v1.v, v2.v, dest->v);
}

void Quat_DivTo(quat_t v1, quat_t v2, quat_t* dest)
{
	quat_t v2_inv = NEW_QUAT(1.0f / v2.x, 1.0f / v2.y, 1.0f / v2.z, 1.0f / v2.w);
	
	glm_quat_mul(v1.v, v2_inv.v, dest->v);
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

void Quat_GetAxisAngle(quat_t* quat, float* angle, vec3_t* axis)
{
	glm_quat_axis(quat->v, axis->v);
	*angle = glm_deg(glm_quat_angle(quat->v));
}
