#include <math/mat4.h>

#include <cglm/cglm.h>

#define GLM_MAT float(*)[4]

void Mat4_Mul(const mat4_t* m1, const mat4_t* m2, mat4_t* dest)
{
	glm_mat4_mul((GLM_MAT)m1, (GLM_MAT)m2, (GLM_MAT)dest);
}

void Mat4_Inv(const mat4_t* mat, mat4_t* dest)
{
	glm_mat4_inv((GLM_MAT)mat, (GLM_MAT)dest);
}

void Mat4_Translate(mat4_t* mat, vec3_t offset)
{
	glm_translate((GLM_MAT)mat, offset.v);
}

void Mat4_Scale(mat4_t* mat, vec3_t scale)
{
	glm_scale((GLM_MAT)mat, scale.v);
}

void Mat4_Rotate(mat4_t* mat, float angle, vec3_t axis)
{
	glm_rotate((GLM_MAT)mat, angle, axis.v);
}

void Mat4_Ortho(float left, float right, float bottom, float top, float nearZ, float farZ, mat4_t* dest)
{
	glm_ortho(left, right, bottom, top, nearZ, farZ, (GLM_MAT)dest);
}

void Mat4_Perspective(float fov, float aspect, float nearZ, float farZ, mat4_t* dest)
{
	glm_perspective(fov, aspect, nearZ, farZ, (GLM_MAT)dest);
}

void Mat4_LookAt(vec3_t eye, vec3_t center, vec3_t up, mat4_t* dest)
{
	glm_lookat(eye.v, center.v, up.v, (GLM_MAT)dest);
}