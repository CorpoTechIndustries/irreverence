#include <math/vec3.h>

#include <cglm/cglm.h>

// ------------------------------
// Vec3

void Vec3_Add(vec3_t v1, vec3_t v2, vec3_t* dest)
{
	glm_vec3_add(v1.v, v2.v, dest->v);
}

void Vec3_Sub(vec3_t v1, vec3_t v2, vec3_t* dest)
{
	glm_vec3_sub(v1.v, v2.v, dest->v);
}

void Vec3_Mul(vec3_t v1, vec3_t v2, vec3_t* dest)
{
	glm_vec3_mul(v1.v, v2.v, dest->v);
}

void Vec3_Div(vec3_t v1, vec3_t v2, vec3_t* dest)
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

// ------------------------------ 
// IVec3

void IVec3_Add(ivec3_t v1, ivec3_t v2, ivec3_t* dest)
{
	glm_ivec3_add(v1.v, v2.v, dest->v);
}

void IVec3_Sub(ivec3_t v1, ivec3_t v2, ivec3_t* dest)
{
	glm_ivec3_sub(v1.v, v2.v, dest->v);
}

void IVec3_Mul(ivec3_t v1, ivec3_t v2, ivec3_t* dest)
{
	glm_ivec3_mul(v1.v, v2.v, dest->v);
}

void IVec3_Div(ivec3_t v1, ivec3_t v2, ivec3_t* dest)
{
	glm_ivec3_div(v1.v, v2.v, dest->v);
}

int IVec3_Dot(ivec3_t v1, ivec3_t v2)
{
	return glm_ivec3_dot(v1.v, v2.v);
}