#include <math/vec4.h>

#include <cglm/cglm.h>

// ------------------------------
// Vec4

vec4_t Vec4_Add(vec4_t v1, vec4_t v2)
{
	vec4_t v3;
	glm_vec4_add(v1.v, v2.v, (&v3)->v);
	
	return v3;
}

vec4_t Vec4_Adds(vec4_t v1, vec_t scale)
{
	vec4_t v3;
	glm_vec4_adds(v1.v, scale, (&v3)->v);

	return v3;
}

vec4_t Vec4_Sub(vec4_t v1, vec4_t v2)
{
	vec4_t v3;
	glm_vec4_sub(v1.v, v2.v, (&v3)->v);
	
	return v3;
}

vec4_t Vec4_Subs(vec4_t v1, vec_t scale)
{
	vec4_t v3;
	glm_vec4_subs(v1.v, scale, (&v3)->v);

	return v3;
}

vec4_t Vec4_Mul(vec4_t v1, vec4_t v2)
{
	vec4_t v3;
	glm_vec4_mul(v1.v, v2.v, (&v3)->v);

	return v3;
}

vec4_t Vec4_Muls(vec4_t v1, vec_t scale)
{
	vec4_t v3;
	glm_vec4_mul(v1.v, NEW_VEC4S(scale).v, v3.v);

	return v3;
}

vec4_t Vec4_Div(vec4_t v1, vec4_t v2)
{
	vec4_t v3;
	glm_vec4_div(v1.v, v2.v, (&v3)->v);

	return v3;
}

vec4_t Vec4_Divs(vec4_t v1, vec_t scale)
{
	vec4_t v3;
	glm_vec4_divs(v1.v, scale, (&v3)->v);

	return v3;
}

void Vec4_AddTo(vec4_t v1, vec4_t v2, vec4_t* dest)
{
	glm_vec4_add(v1.v, v2.v, dest->v);
}

void Vec4_AddsTo(vec4_t v1, vec_t scale, vec4_t* dest)
{
	glm_vec4_adds(v1.v, scale, dest->v);
}

void Vec4_SubTo(vec4_t v1, vec4_t v2, vec4_t* dest)
{
	glm_vec4_sub(v1.v, v2.v, dest->v);
}

void Vec4_SubsTo(vec4_t v1, vec_t scale, vec4_t* dest)
{
	glm_vec4_subs(v1.v, scale, dest->v);
}

void Vec4_MulTo(vec4_t v1, vec4_t v2, vec4_t* dest)
{
	glm_vec4_mul(v1.v, v2.v, dest->v);
}

void Vec4_MulsTo(vec4_t v, vec_t scale, vec4_t* dest)
{
	glm_vec4_mul(v.v, NEW_VEC4S(scale).v, dest->v);
}

void Vec4_DivTo(vec4_t v1, vec4_t v2, vec4_t* dest)
{
	glm_vec4_div(v1.v, v2.v, dest->v);
}

void Vec4_DivsTo(vec4_t v1, vec_t scale, vec4_t* dest)
{
	glm_vec4_divs(v1.v, scale, dest->v);
}

// ------------------------------
// IVec4

ivec4_t IVec4_Add(ivec4_t v1, ivec4_t v2)
{
	ivec4_t v3;
	glm_ivec4_add(v1.v, v2.v, (&v3)->v);

	return v3;
}

ivec4_t IVec4_Adds(ivec4_t v1, ivec_t scale)
{
	ivec4_t v3;
	glm_ivec4_adds(v1.v, scale, (&v3)->v);

	return v3;
}

ivec4_t IVec4_Sub(ivec4_t v1, ivec4_t v2)
{
	ivec4_t v3;
	glm_ivec4_sub(v1.v, v2.v, (&v3)->v);

	return v3;
}

ivec4_t IVec4_Subs(ivec4_t v1, ivec_t scale)
{
	ivec4_t v3;
	glm_ivec4_subs(v1.v, scale, (&v3)->v);

	return v3;
}

ivec4_t IVec4_Mul(ivec4_t v1, ivec4_t v2)
{
	ivec4_t v3;
	glm_ivec4_mul(v1.v, v2.v, (&v3)->v);

	return v3;
}

ivec4_t IVec4_Muls(ivec4_t v1, ivec_t scale)
{
	ivec4_t v3;
	glm_ivec4_mul(v1.v, NEW_IVEC4S(scale).v, v3.v);

	return v3;
}

ivec4_t IVec4_Div(ivec4_t v1, ivec4_t v2)
{
	return NEW_IVEC4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

ivec4_t IVec4_Divs(ivec4_t v1, ivec_t scale)
{
	return NEW_IVEC4(v1.x / scale, v1.y / scale, v1.z / scale, v1.w / scale);
}

void IVec4_AddTo(ivec4_t v1, ivec4_t v2, ivec4_t* dest)
{
	glm_ivec4_add(v1.v, v2.v, dest->v);
}

void IVec4_AddsTo(ivec4_t v1, ivec_t scale, ivec4_t* dest)
{
	glm_ivec4_adds(v1.v, scale, dest->v);
}

void IVec4_SubTo(ivec4_t v1, ivec4_t v2, ivec4_t* dest)
{
	glm_ivec4_sub(v1.v, v2.v, dest->v);
}

void IVec4_SubsTo(ivec4_t v1, ivec_t scale, ivec4_t* dest)
{
	glm_ivec4_subs(v1.v, scale, dest->v);
}

void IVec4_MulTo(ivec4_t v1, ivec4_t v2, ivec4_t* dest)
{
	glm_ivec4_mul(v1.v, v2.v, dest->v);
}

void IVec4_MulsTo(ivec4_t v, ivec_t scale, ivec4_t* dest)
{
	glm_ivec4_mul(v.v, NEW_IVEC4S(scale).v, dest->v);
}

void IVec4_DivTo(ivec4_t v1, ivec4_t v2, ivec4_t* dest)
{
	*dest = NEW_IVEC4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

void IVec4_DivsTo(ivec4_t v1, ivec_t scale, ivec4_t* dest)
{
	*dest = NEW_IVEC4(v1.x / scale, v1.y / scale, v1.z / scale, v1.w / scale);
}
