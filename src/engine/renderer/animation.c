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

static void ConvertMat4(const struct aiMatrix4x4* aimat, dualquat_t* dest)
{
    mat4_t mat = {
		aimat->a1, aimat->b1, aimat->c1, aimat->d1,
		aimat->a2, aimat->b2, aimat->c2, aimat->d2,
		aimat->a3, aimat->b3, aimat->c3, aimat->d3,
		aimat->a4, aimat->b4, aimat->c4, aimat->d4
    };

	Mat4_ToDualQuat(&mat, dest);
}

static float GetScaleFactor(float time, float next_time, float anim_time)
{
	float midwayLength = anim_time - time;
	float framesDiff = next_time - time;
	return midwayLength / framesDiff;
}

static void InterpolatePosition(anim_bone_t* animbone, float anim_time, const vec3_t* offset, mat4_t* dest)
{
	if (animbone->translationCount == 1) {
		Mat4_Translate(dest, animbone->translations[0].val);
		return;
	}
	
	uint32_t p0 = 0;
	for (uint32_t i = 0; i < animbone->translationCount - 1; i++) {
		if (anim_time < animbone->translations[i + 1].time) {
			p0 = i;
			break;
		}
	}

	uint32_t p1 = p0 + 1;

	float scaleFactor = GetScaleFactor(animbone->translations[p0].time, animbone->translations[p1].time, anim_time);

	vec3_t finalPosition = VEC3_ZERO;
	Vec3_Lerp(animbone->translations[p0].val, animbone->translations[p1].val, scaleFactor, &finalPosition);

	Vec3_AddTo(finalPosition, *offset, &finalPosition);

	Mat4_Translate(dest, finalPosition);
}

static void InterpolateRotation(anim_bone_t* animbone, float anim_time, const quat_t* offset, mat4_t* dest)
{
	if (animbone->rotationCount == 1) {
		Quat_ToMat4(animbone->rotations[0].val, dest);
		return;
	}
	
	uint32_t p0 = 0;
	for (uint32_t i = 0; i < animbone->rotationCount - 1; i++) {
		if (anim_time < animbone->rotations[i + 1].time) {
			p0 = i;
			break;
		}
	}

	uint32_t p1 = p0 + 1;

	float scaleFactor = GetScaleFactor(animbone->rotations[p0].time, animbone->rotations[p1].time, anim_time);

	quat_t finalRotation;
	Quat_SLerp(animbone->rotations[p0].val, animbone->rotations[p1].val, scaleFactor, &finalRotation);
	
	Quat_MulTo(finalRotation, *offset, &finalRotation);

	Quat_ToMat4(finalRotation, dest);
}

static void AnimationBone_Init(anim_bone_t* animbone, bone_info_t* info, const void* channel)
{
	const struct aiNodeAnim* aichannel = channel;

	animbone->boneInfo = info;

	animbone->translationCount = aichannel->mNumPositionKeys;
	if (aichannel->mNumPositionKeys > 0) {
		animbone->translations = Sys_Malloc(sizeof(keyframe_vec3_t) * aichannel->mNumPositionKeys);
	} else {
		animbone->translations = NULL;
	}

	animbone->rotationCount = aichannel->mNumRotationKeys;
	if (aichannel->mNumRotationKeys > 0) {
		animbone->rotations = Sys_Malloc(sizeof(keyframe_quat_t) * aichannel->mNumRotationKeys);
	} else {
		animbone->rotations = NULL;
	}
	
	for (uint32_t i = 0; i < aichannel->mNumPositionKeys; i++) {
		keyframe_vec3_t* keyframe = &animbone->translations[i];
		const struct aiVectorKey* aikey = &aichannel->mPositionKeys[i];

		keyframe->val = *((vec3_t*)&aikey->mValue);
		keyframe->time = (float)aikey->mTime;
	}	

	for (uint32_t i = 0; i < aichannel->mNumRotationKeys; i++) {
		keyframe_quat_t* keyframe = &animbone->rotations[i];
		const struct aiQuatKey* aikey = &aichannel->mRotationKeys[i];

		keyframe->val = NEW_QUAT(aikey->mValue.x, aikey->mValue.y, aikey->mValue.z, aikey->mValue.w);
		keyframe->time = (float)aikey->mTime;
	}	
}

static void AnimationBone_Destroy(anim_bone_t* animbone)
{
	if (animbone->translations) {
		Sys_Free(animbone->translations);
	}

	if (animbone->rotations) {
		Sys_Free(animbone->rotations);
	}
}

static void ReadHierarchyData(anim_node_t* root, const struct aiNode* src)
{
	if (!src) {
		LOG_ERROR("Source Node doesn't exist");
		return;
	}

	Sys_MemZero(root->name, MAX_ANIM_NODE_NAME_LENGTH);
	if (src->mName.length > 0) {
		strncpy(root->name, src->mName.data, MATH_MIN(src->mName.length, MAX_ANIM_NODE_NAME_LENGTH));
	}

	root->childrenCount = src->mNumChildren;

	if (root->childrenCount > 0) {
		root->childrens = Array_Create(anim_node_t);
	} else {
		root->childrens = NULL;
	}

	ConvertMat4(&src->mTransformation, &root->transform);
	
	for (uint32_t i = 0; i < root->childrenCount; i++) {
		anim_node_t newNode;
		ReadHierarchyData(&newNode, src->mChildren[i]);
		Array_Push(root->childrens, newNode);
	}
}

static void FreeHierarchData(anim_node_t* root)
{
	for (uint32_t i = 0; i < root->childrenCount; i++) {
		FreeHierarchData(&root->childrens[i]);
	}
	
	if (root->childrens) {
		Array_Destroy(root->childrens);
		root->childrens = NULL;
	}
}

static void ReadAnimationBones(animation_t* animation, const struct aiAnimation* ai_anim, model_t* model)
{
	animation->bones.count = ai_anim->mNumChannels;	
	
	if (animation->bones.count > 0) {
		animation->bones.list = Sys_Malloc(sizeof(anim_bone_t) * animation->bones.count);	
	}

	for (uint8_t i = 0; i < ai_anim->mNumChannels; i++) {
		const struct aiNodeAnim* ainode_anim = ai_anim->mChannels[i];

		bone_info_t* boneinfo = Model_FindBoneInfo(model, ainode_anim->mNodeName.data);

		if (!boneinfo) {
			bone_info_t newBoneinfo;
			newBoneinfo.id = model->bones.count;

			Sys_MemZero(newBoneinfo.name, MAX_BONEINFO_NAME_LENGTH);
			if (ainode_anim->mNodeName.length > 0) {
				strncpy(newBoneinfo.name, ainode_anim->mNodeName.data, MATH_MIN(ainode_anim->mNodeName.length, MAX_BONEINFO_NAME_LENGTH));
			}

			newBoneinfo.offset = DUALQUAT_IDENTITY;

			Array_Push(model->bones.list, newBoneinfo);
			
			boneinfo = &model->bones.list[newBoneinfo.id];

			model->bones.count++;
		}

		AnimationBone_Init(&animation->bones.list[i], boneinfo, ainode_anim);
	}
}

bool Animation_Init(animation_t* animation, model_t* model, const void* scene, uint32_t index)
{

	return true;
}

bool Animation_InitFromPath(animation_t* animation, model_t* model, const char* path, uint32_t index)
{
	animation->bones.list = NULL;
	animation->bones.count = 0;

	animation->duration = 0.0f;
	animation->tps = 0;
	animation->rootNode = (anim_node_t) { 0 };
	animation->model = model;

	if (!path) {
		return false;
	}

	const char* fileExtension = Sys_PathGetExtension(path);
	if (!fileExtension || aiIsExtensionSupported(fileExtension) == AI_FALSE) {
		LOG_ERROR("Animation Model path does not have a valid or supported extension");
		return false;
	}

	const struct aiScene* scene = NULL;
	scene = aiImportFile(path, 0);

	if (!scene) {
		LOG_ERROR("Animation Model could not be loaded");
		return false;
	}

	if (index >= scene->mNumAnimations) {
		LOG_ERROR("Model has no Animations at index %i", index);
		return false;
	}

	const struct aiAnimation* aianim = scene->mAnimations[index];

	animation->duration = (float)aianim->mDuration;
	animation->tps = (float)aianim->mTicksPerSecond;

	ReadHierarchyData(&animation->rootNode, scene->mRootNode);

	ReadAnimationBones(animation, aianim, model);

	aiReleaseImport(scene);

	return true;
}

static anim_bone_inst_t* Animator_FindBoneInstance(animator_t* animator, const char* name)
{
	for (uint8_t i = 0; i < animator->animation->bones.count; i++) {
		anim_bone_inst_t* boneInst = &animator->boneInstances[i];
		if (strncmp(boneInst->bone->boneInfo->name, name, MAX_BONEINFO_NAME_LENGTH) == 0) {
			return boneInst;
		}
	}

	return NULL;
}

void Animation_Destroy(animation_t* animation)
{
	if (animation->bones.count > 0) {
		for (uint8_t i = 0; i < animation->bones.count; i++) {
			AnimationBone_Destroy(&animation->bones.list[i]);
		}
		
		Sys_Free(animation->bones.list);
		animation->bones.list = NULL;
	}

	FreeHierarchData(&animation->rootNode);
}

static void CalcBoneTransform(animator_t* animator, const anim_node_t* node, dualquat_t* parent_transform)
{
	anim_bone_inst_t* boneInst = Animator_FindBoneInstance(animator, node->name);

	dualquat_t nodeTransform;
	bone_info_t* boneinfo = NULL;

	if (boneInst) {
		boneinfo = boneInst->bone->boneInfo;

		mat4_t translation = MAT4_IDENTITY;
		mat4_t rotation = MAT4_IDENTITY;
		mat4_t scale = MAT4_IDENTITY;
		InterpolatePosition(boneInst->bone, animator->time, &animator->boneOffsetPositions[boneinfo->id], &translation);

		InterpolateRotation(boneInst->bone, animator->time, &animator->boneOffsetRotations[boneinfo->id], &rotation);	
		Mat4_Mul(&translation, &rotation, &boneInst->localTrans);

		Mat4_ToDualQuat(&boneInst->localTrans, &nodeTransform);
	} else {
		nodeTransform = node->transform;
	}
	
	dualquat_t globalTransform;
	DualQuat_MulTo(parent_transform, &nodeTransform, &globalTransform);

	if (boneinfo) {
		DualQuat_MulTo(&globalTransform, &boneinfo->offset, &animator->boneQuats[boneinfo->id]);
	}

	for (uint32_t i = 0; i < node->childrenCount; i++) {
		CalcBoneTransform(animator, &node->childrens[i], &globalTransform);
	}	
}

void Animator_Init(animator_t* animator, animation_t* animation)
{
	animator->animation = animation;
	animator->time = 0.0f;
	animator->boneInstances = NULL;
	animator->boneCount = animation->bones.count;
	
	animator->boneQuats = Sys_Malloc(sizeof(dualquat_t) * animator->boneCount);
	for (uint32_t i = 0; i < animator->boneCount; i++) {
		animator->boneQuats[i] = DUALQUAT_IDENTITY;
	}

	animator->boneOffsetPositions = Sys_Calloc(sizeof(vec3_t) * animator->boneCount);

	animator->boneOffsetRotations = Sys_Malloc(sizeof(quat_t) * animator->boneCount);
	for (uint32_t i = 0; i < animator->boneCount; i++) {
		animator->boneOffsetRotations[i] = QUAT_IDENTITY;
	}

	animator->boneInstances = Sys_Malloc(sizeof(anim_bone_inst_t) * animator->boneCount);
	for (uint32_t i = 0; i < animator->boneCount; i++) {
		anim_bone_inst_t* boneInst = &animator->boneInstances[i];
		boneInst->bone = &animation->bones.list[i];
		boneInst->localTrans = MAT4_IDENTITY;
	}	
}

void Animator_Update(animator_t* animator, float frametime)
{
	if (animator->animation) {
		animator->time += animator->animation->tps * frametime;
		animator->time = fmodf(animator->time, animator->animation->duration);
		
		dualquat_t parentIdentity = DUALQUAT_IDENTITY;
		CalcBoneTransform(animator, &animator->animation->rootNode, &parentIdentity);
	}
}

void Animator_Play(animator_t* animator, animation_t* animation)
{
	animator->animation = animation;
	animator->time = 0.0f;

	for (uint32_t i = 0; i < animation->bones.count; i++) {
		anim_bone_inst_t* boneInst = &animator->boneInstances[i];
		boneInst->bone = &animation->bones.list[i];
		boneInst->localTrans = MAT4_IDENTITY;
	}

	for (uint32_t i = 0; i < MAX_MODEL_BONES; i++) {
		animator->boneQuats[i] = DUALQUAT_IDENTITY;
	}
}

void Animator_Stop(animator_t* animator)
{
	if (!animator->animation) {
		return;
	}

	animator->animation = NULL;
	animator->time = 0.0f;

	for (uint32_t i = 0; i < MAX_MODEL_BONES; i++) {
		animator->boneQuats[i] = DUALQUAT_IDENTITY;
	}
}

void Animator_Destroy(animator_t* animator)
{
	if (animator->boneQuats) {
		Sys_Free(animator->boneQuats);
		animator->boneQuats = NULL;
	}

	if (animator->boneOffsetPositions) {
		Sys_Free(animator->boneOffsetPositions);
		animator->boneOffsetPositions = NULL;
	}

	if (animator->boneOffsetRotations) {
		Sys_Free(animator->boneOffsetRotations);
		animator->boneOffsetRotations = NULL;
	}

	if (animator->boneInstances) {
		Sys_Free(animator->boneInstances);
		animator->boneInstances = NULL;
	}
}
