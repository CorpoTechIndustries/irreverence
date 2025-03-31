#include <engine/renderer/animation.h>

#include <engine/renderer/renderer.h>
#include <engine/renderer/model.h>
#include <engine/log.h>

#include <util/array.h>

#include <platform/path.h>
#include <platform/memory.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static void ConvertMat4(const struct aiMatrix4x4* mat, mat4_t* dest) // Duplicate of the one in model.c, should unify these. Maybe Mat4_Transpose()?
{
	dest->m0 = mat->a1; dest->m4 = mat->a2; dest->m8 = mat->a3;  dest->m12 = mat->a4;
	dest->m1 = mat->b1; dest->m5 = mat->b2; dest->m9 = mat->b3;  dest->m13 = mat->b4;
	dest->m2 = mat->c1; dest->m6 = mat->c2; dest->m10 = mat->c3; dest->m14 = mat->c4;
	dest->m3 = mat->d1; dest->m7 = mat->d2; dest->m11 = mat->d3; dest->m15 = mat->d4;
}

static float GetScaleFactor(float time, float next_time, float anim_time)
{
	float midwayLength = anim_time - time;
	float framesDiff = next_time - time;
	return midwayLength / framesDiff;
}
