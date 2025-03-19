#include <math/vec3.h>

#include <cglm/cglm.h>

// ------------------------------
// Vec3

vec3_t Vec3_Add(vec3_t v1, vec3_t v2)
{
	vec3_t v3;
	glm_vec3_add(v1.v, v2.v, (&v3)->v);

	return v3;
}

vec3_t Vec3_Sub(vec3_t v1, vec3_t v2)
{
	vec3_t v3;
	glm_vec3_sub(v1.v, v2.v, (&v3)->v);

	return v3;
}

vec3_t Vec3_Mul(vec3_t v1, vec3_t v2)
{
	vec3_t v3;
	glm_vec3_mul(v1.v, v2.v, (&v3)->v);

	return v3;
}

vec3_t Vec3_Muls(vec3_t v, vec_t scale)
{
	vec3_t dest;
	glm_vec3_mul(v.v, NEW_VEC3S(scale).v, dest.v);

	return dest;
}

vec3_t Vec3_Div(vec3_t v1, vec3_t v2)
{
	vec3_t v3;
	glm_vec3_div(v1.v, v2.v, (&v3)->v);

	return v3;
}

void Vec3_AddTo(vec3_t v1, vec3_t v2, vec3_t* dest)
{
	glm_vec3_add(v1.v, v2.v, dest->v);
}

void Vec3_SubTo(vec3_t v1, vec3_t v2, vec3_t* dest)
{
	glm_vec3_sub(v1.v, v2.v, dest->v);
}

void Vec3_MulTo(vec3_t v1, vec3_t v2, vec3_t* dest)
{
	glm_vec3_mul(v1.v, v2.v, dest->v);
}

void Vec3_MulsTo(vec3_t v, vec_t scale, vec3_t* dest)
{
	glm_vec3_mul(v.v, NEW_VEC3S(scale).v, dest->v);
}

void Vec3_DivTo(vec3_t v1, vec3_t v2, vec3_t* dest)
{
	glm_vec3_div(v1.v, v2.v, dest->v);
}

void Vec3_Normalize(vec3_t vec, vec3_t* dest)
{
	glm_vec3_normalize_to(vec.v, dest->v);
}

void Vec3_Cross(vec3_t v1, vec3_t v2, vec3_t* dest)
{
	glm_vec3_cross(v1.v, v2.v, dest->v);
}

float Vec3_Dot(vec3_t v1, vec3_t v2)
{
	return glm_vec3_dot(v1.v, v2.v);
}

void Vec3_Rotate(float angle, vec3_t axis, vec3_t* dest)
{
	glm_vec3_rotate(dest->v, glm_rad(angle), axis.v);
}

// ------------------------------
// IVec3

ivec3_t IVec3_Add(ivec3_t v1, ivec3_t v2)
{
	ivec3_t v3;
	IVec3_AddTo(v1, v2, &v3);

	return v3;
}

ivec3_t IVec3_Sub(ivec3_t v1, ivec3_t v2)
{
	ivec3_t v3;
	IVec3_SubTo(v1, v2, &v3);

	return v3;
}

ivec3_t IVec3_Mul(ivec3_t v1, ivec3_t v2)
{
	ivec3_t v3;
	IVec3_MulTo(v1, v2, &v3);

	return v3;
}

ivec3_t IVec3_Div(ivec3_t v1, ivec3_t v2)
{
	ivec3_t v3;
	IVec3_DivTo(v1, v2, &v3);

	return v3;
}

void IVec3_AddTo(ivec3_t v1, ivec3_t v2, ivec3_t* dest)
{
	glm_ivec3_add(v1.v, v2.v, dest->v);
}

void IVec3_SubTo(ivec3_t v1, ivec3_t v2, ivec3_t* dest)
{
	glm_ivec3_sub(v1.v, v2.v, dest->v);
}

void IVec3_MulTo(ivec3_t v1, ivec3_t v2, ivec3_t* dest)
{
	glm_ivec3_mul(v1.v, v2.v, dest->v);
}

void IVec3_DivTo(ivec3_t v1, ivec3_t v2, ivec3_t* dest)
{
	glm_ivec3_div(v1.v, v2.v, dest->v);
}

int IVec3_Dot(ivec3_t v1, ivec3_t v2)
{
	return glm_ivec3_dot(v1.v, v2.v);
}
