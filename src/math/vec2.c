#include <math/vec2.h>

#include <cglm/cglm.h>

// ------------------------------
// Vec2

vec2_t Vec2_Add(vec2_t v1, vec2_t v2)
{
	vec2_t v3;
	glm_vec2_add(v1.v, v2.v, (&v3)->v);

	return v3;
}

vec2_t Vec2_Adds(vec2_t v1, vec_t scale)
{
	vec2_t v3;
	glm_vec2_adds(v1.v, scale, (&v3)->v);

	return v3;
}

vec2_t Vec2_Sub(vec2_t v1, vec2_t v2)
{
	vec2_t v3;
	glm_vec2_sub(v1.v, v2.v, (&v3)->v);

	return v3;
}

vec2_t Vec2_Subs(vec2_t v1, vec_t scale)
{
	vec2_t v3;
	glm_vec2_subs(v1.v, scale, (&v3)->v);

	return v3;
}

vec2_t Vec2_Mul(vec2_t v1, vec2_t v2)
{
	vec2_t v3;
	glm_vec2_mul(v1.v, v2.v, (&v3)->v);

	return v3;
}

vec2_t Vec2_Muls(vec2_t v1, vec_t scale)
{
	vec2_t vec3;
	glm_vec2_mul(v1.v, NEW_VEC2S(scale).v, vec3.v);

	return vec3;
}

vec2_t Vec2_Div(vec2_t v1, vec2_t v2)
{
	vec2_t v3;
	glm_vec2_div(v1.v, v2.v, (&v3)->v);

	return v3;
}

vec2_t Vec2_Divs(vec2_t v1, vec_t scale)
{
	vec2_t v3;
	glm_vec2_divs(v1.v, scale, (&v3)->v);

	return v3;
}

void Vec2_AddTo(vec2_t v1, vec2_t v2, vec2_t* dest)
{
	glm_vec2_add(v1.v, v2.v, dest->v);

}

void Vec2_AddsTo(vec2_t v1, vec_t scale, vec2_t* dest)
{
	glm_vec2_adds(v1.v, scale, dest->v);
}

void Vec2_SubTo(vec2_t v1, vec2_t v2, vec2_t* dest)
{
	glm_vec2_sub(v1.v, v2.v, dest->v);
}

void Vec2_SubsTo(vec2_t v1, vec_t scale, vec2_t* dest)
{
	glm_vec2_subs(v1.v, scale, dest->v);
}

void Vec2_MulTo(vec2_t v1, vec2_t v2, vec2_t* dest)
{
	glm_vec2_mul(v1.v, v2.v, dest->v);
}

void Vec2_MulsTo(vec2_t v1, vec_t scale, vec2_t* dest)
{
	glm_vec2_mul(v1.v, NEW_VEC2S(scale).v, dest->v);
}

void Vec2_DivTo(vec2_t v1, vec2_t v2, vec2_t* dest)
{
	glm_vec2_div(v1.v, v2.v, dest->v);
}

void Vec2_DivsTo(vec2_t v1, vec_t scale, vec2_t* dest)
{
	glm_vec2_divs(v1.v, scale, dest->v);
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

ivec2_t IVec2_Add(ivec2_t v1, ivec2_t v2)
{
	ivec2_t v3;
	glm_ivec2_add(v1.v, v2.v, (&v3)->v);

	return v3;
}

ivec2_t IVec2_Adds(ivec2_t v1, ivec_t scale)
{
	ivec2_t v3;
	glm_ivec2_adds(v1.v, scale, (&v3)->v);

	return v3;
}

ivec2_t IVec2_Sub(ivec2_t v1, ivec2_t v2)
{
	ivec2_t v3;
	glm_ivec2_sub(v1.v, v2.v, (&v3)->v);

	return v3;
}

ivec2_t IVec2_Subs(ivec2_t v1, ivec_t scale)
{
	ivec2_t v3;
	glm_ivec2_subs(v1.v, scale, (&v3)->v);

	return v3;
}

ivec2_t IVec2_Mul(ivec2_t v1, ivec2_t v2)
{
	ivec2_t v3;
	glm_ivec2_mul(v1.v, v2.v, (&v3)->v);

	return v3;
}

ivec2_t IVec2_Muls(ivec2_t v1, ivec_t scale)
{
	ivec2_t v3;
	glm_ivec2_mul(v1.v, NEW_IVEC2S(scale).v, v3.v);

	return v3;
}

ivec2_t IVec2_Div(ivec2_t v1, ivec2_t v2)
{
	ivec2_t v3;
	glm_ivec2_div(v1.v, v2.v, (&v3)->v);

	return v3;
}

ivec2_t IVec2_Divs(ivec2_t v1, ivec_t scale)
{
	ivec2_t v3;
	glm_ivec2_divs(v1.v, scale, (&v3)->v);

	return v3;
}

void IVec2_AddTo(ivec2_t v1, ivec2_t v2, ivec2_t* dest)
{
	glm_ivec2_add(v1.v, v2.v, dest->v);
}

void IVec2_AddsTo(ivec2_t v1, ivec_t scale, ivec2_t* dest)
{
	glm_ivec2_adds(v1.v, scale, dest->v);
}

void IVec2_SubTo(ivec2_t v1, ivec2_t v2, ivec2_t* dest)
{
	glm_ivec2_sub(v1.v, v2.v, dest->v);
}

void IVec2_SubsTo(ivec2_t v1, ivec_t scale, ivec2_t* dest)
{
	glm_ivec2_subs(v1.v, scale, dest->v);
}

void IVec2_MulTo(ivec2_t v1, ivec2_t v2, ivec2_t* dest)
{
	glm_ivec2_mul(v1.v, v2.v, dest->v);
}

void IVec2_MulsTo(ivec2_t v1, ivec_t scale, ivec2_t* dest)
{
	glm_ivec2_mul(v1.v, NEW_IVEC2S(scale).v, dest->v);
}

void IVec2_DivTo(ivec2_t v1, ivec2_t v2, ivec2_t* dest)
{
	glm_ivec2_div(v1.v, v2.v, dest->v);
}

void IVec2_DivsTo(ivec2_t v1, ivec_t scale, ivec2_t* dest)
{
	glm_ivec2_divs(v1.v, scale, dest->v);
}

int IVec2_Dot(ivec2_t v1, ivec2_t v2)
{
	return glm_ivec2_dot(v1.v, v2.v);
}
