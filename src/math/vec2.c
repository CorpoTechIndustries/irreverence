#include <math/vec2.h>

#include <cglm/cglm.h>

// ------------------------------
// Vec2

void Vec2_Add(vec2_t v1, vec2_t v2, vec2_t* dest)
{
	glm_vec2_add(v1.v, v2.v, dest->v);
}

void Vec2_Sub(vec2_t v1, vec2_t v2, vec2_t* dest)
{
	glm_vec2_sub(v1.v, v2.v, dest->v);
}

void Vec2_Mul(vec2_t v1, vec2_t v2, vec2_t* dest)
{
	glm_vec2_mul(v1.v, v2.v, dest->v);
}

void Vec2_Div(vec2_t v1, vec2_t v2, vec2_t* dest)
{
	glm_vec2_div(v1.v, v2.v, dest->v);
}

void Vec2_Normalize(vec2_t vec, vec2_t* dest)
{
	glm_vec2_normalize_to(vec.v, dest->v);
}

float Vec2_Cross(vec2_t v1, vec2_t v2)
{
	return glm_vec2_cross(v1.v, v2.v);
}

float Vec2_Dot(vec2_t v1, vec2_t v2)
{
	return glm_vec2_dot(v1.v, v2.v);
}

// ------------------------------
// IVec2

void IVec2_Add(ivec2_t v1, ivec2_t v2, ivec2_t* dest)
{
	glm_ivec2_add(v1.v, v2.v, dest->v);
}

void IVec2_Sub(ivec2_t v1, ivec2_t v2, ivec2_t* dest)
{
	glm_ivec2_sub(v1.v, v2.v, dest->v);
}

void IVec2_Mul(ivec2_t v1, ivec2_t v2, ivec2_t* dest)
{
	glm_ivec2_mul(v1.v, v2.v, dest->v);
}

void IVec2_Div(ivec2_t v1, ivec2_t v2, ivec2_t* dest)
{
	glm_ivec2_div(v1.v, v2.v, dest->v);
}

int IVec2_Dot(ivec2_t v1, ivec2_t v2)
{
	return glm_ivec2_dot(v1.v, v2.v);
}
