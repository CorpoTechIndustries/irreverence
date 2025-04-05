#pragma once

#include <math/mat4.h>
#include <math/vec3.h>
#include <math/quat.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

#define MAX_ANIM_NODE_NAME_LENGTH 32

typedef struct bone_info bone_info_t;
typedef struct model model_t;

typedef struct {
	vec3_t val;
	float time;
} keyframe_vec3_t;

typedef struct {
	quat_t val;
	float time; 
} keyframe_quat_t;

typedef struct {
	bone_info_t* boneInfo;
	keyframe_vec3_t* translations;
	keyframe_quat_t* rotations;
	uint32_t translationCount;
	uint32_t rotationCount;
} anim_bone_t;

typedef struct {
	mat4_t localTrans;
	anim_bone_t* bone;
} anim_bone_inst_t;

typedef struct anim_node {
	dualquat_t transform;
	struct anim_node* childrens;
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
		uint32_t count;
	} bones;
} animation_t; // Will be stored on model_t

typedef struct {
	animation_t* animation;
	dualquat_t* boneQuats;
	vec3_t* boneOffsetPositions;
	quat_t* boneOffsetRotations;
	anim_bone_inst_t* boneInstances;
	float time;
	uint8_t boneCount;	
} animator_t;

bool Animation_Init(animation_t* animation, model_t* model, const void* scene, uint32_t index);
bool Animation_InitFromPath(animation_t* animation, model_t* model, const char* path, uint32_t index);
void Animation_Destroy(animation_t* animation);

void Animator_Init(animator_t* animator, animation_t* animation);
void Animator_Update(animator_t* animator, float frametime);
void Animator_Play(animator_t* animator, animation_t* animation);
void Animator_Stop(animator_t* animator);
void Animator_Destroy(animator_t* animator);
