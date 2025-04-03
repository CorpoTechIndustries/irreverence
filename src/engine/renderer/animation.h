#pragma once

#include <math/mat4.h>
#include <math/vec3.h>
#include <math/quat.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

#define MAX_ANIM_NODE_NAME_LENGTH 32
#define MAX_ANIM_BONE_NAME_LENGTH 32

typedef struct BoneInfo bone_info_t;
typedef struct Model model_t;

typedef struct {
	vec3_t val;
	float time;
} keyframe_vec3_t;

typedef struct {
	quat_t val;
	float time; 
} keyframe_quat_t;

typedef struct {
	mat4_t* localTrans;
	bone_info_t* boneInfo;
	keyframe_vec3_t* translations;
	keyframe_vec3_t* scales;
	keyframe_quat_t* rotations;
	uint32_t translationCount;
	uint32_t scaleCount;
	uint32_t rotationCount;
} anim_bone_t;

typedef struct AnimationNode {
	mat4_t transform;
	struct AnimationNode* childrens;
	uint32_t childrenCount;
	char name[MAX_ANIM_NODE_NAME_LENGTH];
} anim_node_t;

typedef struct {
	anim_node_t rootNode; // TODO: Store this on the model instead
	float duration;
	float tps;
	model_t* model;
	struct {
		anim_bone_t* list;
		mat4_t* transforms;
		uint32_t count;
	} bones;
} animation_t; // Will be stored on model_t

typedef struct {
	animation_t* animation;
	mat4_t* finalMatrices;
	float time;
	float frametime;
} animator_t;

void AnimationBone_Init(anim_bone_t* animbone, mat4_t* mat, bone_info_t* info, const void* channel);
void AnimationBone_Update(anim_bone_t* animbone, float anim_time);
void AnimationBone_Destroy(anim_bone_t* animbone);

bool Animation_Init(animation_t* animation, model_t* model, const void* scene, uint32_t index);
bool Animation_InitFromPath(animation_t* animation, model_t* model, const char* path, uint32_t index);
anim_bone_t* Animation_FindBone(animation_t* animation, const char* name);
void Animation_Destroy(animation_t* animation);

void Animator_Init(animator_t* animator, animation_t* animation);
void Animator_Update(animator_t* animator, float frametime);
void Animator_Destroy(animator_t* animator);
