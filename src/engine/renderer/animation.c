#include <engine/renderer/animation.h>

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

static void InterpolateTranslations(anim_bone_t* animbone, float anim_time, mat4_t* dest)
{	
	uint32_t translationCount = Array_Size(animbone->translations);
	if (translationCount == 1) {
		Mat4_Translate(dest, animbone->translations[0].val);
		return;
	}

	uint32_t i1 = 0;
	for (uint32_t i = 0; i < translationCount - 1; i++) {
		if (anim_time < animbone->translations[i + 1].time) {
			i1 = i;
			break;
		}
	}

	uint32_t i2 = i1 + 1;

	keyframe_vec3_t* p1 = &animbone->translations[i1];
	keyframe_vec3_t* p2 = &animbone->translations[i2];

	float scaleFactor = GetScaleFactor(p1->time, p2->time, anim_time);
	
	vec3_t finalTranslation;
	Vec3_Lerp(p1->val, p2->val, scaleFactor, &finalTranslation);

	Mat4_Translate(dest, finalTranslation);
}

static void InterpolateScales(anim_bone_t* animbone, float anim_time, mat4_t* dest)
{	
	uint32_t scaleCount = Array_Size(animbone->scales);
	if (scaleCount == 1) {
		Mat4_Scale(dest, animbone->scales[0].val);
		return;
	}

	uint32_t i1 = 0;
	for (uint32_t i = 0; i < scaleCount - 1; i++) {
		if (anim_time < animbone->scales[i + 1].time) {
			i1 = i;
			break;
		}
	}

	uint32_t i2 = i1 + 1;

	keyframe_vec3_t* p1 = &animbone->scales[i1];
	keyframe_vec3_t* p2 = &animbone->scales[i2];

	float scaleFactor = GetScaleFactor(p1->time, p2->time, anim_time);
	
	vec3_t finalScale;
	Vec3_Lerp(p1->val, p2->val, scaleFactor, &finalScale);

	Mat4_Scale(dest, finalScale);
}

static void InterpolateRotations(anim_bone_t* animbone, float anim_time, mat4_t* dest)
{	
	uint32_t rotationCount = Array_Size(animbone->rotations);
	if (rotationCount == 1) {
		Mat4_RotateQuat(dest, animbone->rotations[0].val);
		return;
	}

	uint32_t i1 = 0;
	for (uint32_t i = 0; i < rotationCount - 1; i++) {
		if (anim_time < animbone->rotations[i + 1].time) {
			i1 = i;
			break;
		}
	}

	uint32_t i2 = i1 + 1;

	keyframe_quat_t* p1 = &animbone->rotations[i1];
	keyframe_quat_t* p2 = &animbone->rotations[i2];

	float scaleFactor = GetScaleFactor(p1->time, p2->time, anim_time);
	
	quat_t finalRotation;
	Quat_SLerp(p1->val, p2->val, scaleFactor, &finalRotation);

	Mat4_RotateQuat(dest, finalRotation);
}

void AnimationBone_Init(anim_bone_t* animbone, bone_info_t* info, const void* channel)
{
	const struct aiNodeAnim* aiChan = channel;

	animbone->boneInfo = info;
	animbone->translations = NULL;
	animbone->scales = NULL;
	animbone->rotations = NULL;
	animbone->localTrans = MAT4_IDENTITY;

	if (aiChan->mNumPositionKeys > 0) {
		animbone->translations = Array_CreateSize(keyframe_vec3_t, aiChan->mNumPositionKeys); 	
	}

	if (aiChan->mNumScalingKeys > 0) {
		animbone->scales = Array_CreateSize(keyframe_vec3_t, aiChan->mNumRotationKeys); 
	}

	if (aiChan->mNumRotationKeys > 0) {
		animbone->rotations = Array_CreateSize(keyframe_quat_t, aiChan->mNumRotationKeys); 	
	}

	for (uint32_t i = 0; i < aiChan->mNumPositionKeys; i++) {
		const struct aiVectorKey* key = &aiChan->mPositionKeys[i];
		
		animbone->translations[i].val = *((vec3_t*)&key->mValue);
		animbone->translations[i].time = key->mTime;
	}

	for (uint32_t i = 0; i < aiChan->mNumScalingKeys; i++) {
		const struct aiVectorKey* key = &aiChan->mScalingKeys[i];
		
		animbone->scales[i].val = *((vec3_t*)&key->mValue);
		animbone->scales[i].time = key->mTime;
	}
		
	for (uint32_t i = 0; i < aiChan->mNumRotationKeys; i++) {
		const struct aiQuatKey* key = &aiChan->mRotationKeys[i];
		
		animbone->rotations[i].val = *((quat_t*)&key->mValue);
		animbone->rotations[i].time = key->mTime;
	}		
}

void AnimationBone_Update(anim_bone_t* animbone, float anim_time)
{
	mat4_t translation = MAT4_IDENTITY;
	InterpolateTranslations(animbone, anim_time, &translation);

	mat4_t scale = MAT4_IDENTITY;
	InterpolateScales(animbone, anim_time, &scale);
	
	mat4_t rotation = MAT4_IDENTITY;
	InterpolateRotations(animbone, anim_time, &rotation);

	// localTrans = T * R * S
	Mat4_Mul(&translation, &rotation, &translation);
	Mat4_Mul(&translation, &scale, &animbone->localTrans);
}

void AnimationBone_Destroy(anim_bone_t* animbone)
{
	if (animbone->translations) {
		Array_Destroy(animbone->translations);
	}
	
	if (animbone->scales) {
		Array_Destroy(animbone->scales);
	}
	
	if (animbone->rotations) {
		Array_Destroy(animbone->rotations);
	}
}

static void RecursiveNodeData(animation_t* animation, anim_node_t* dest, const struct aiNode* src)
{
	if (!src) {
		LOG_ERROR("SRC?");
	}

	uint32_t childCount = src->mNumChildren;
	dest->childrenCount = childCount;

	if (src->mName.length > 0) {
		strncpy(dest->name, src->mName.data, MATH_MIN(src->mName.length, MAX_ANIM_NODE_NAME_LENGTH));
	} else {
		Sys_MemZero(dest->name, MAX_ANIM_NODE_NAME_LENGTH);
	}
	if (dest->childrenCount > 0) {
		dest->childrens = Array_CreateSize(anim_node_t, dest->childrenCount);
	} else {
		dest->childrens = NULL;
	}

	ConvertMat4(&src->mTransformation, &dest->transform);

	for (uint32_t i = 0; i < childCount; i++) {
		RecursiveNodeData(animation, &dest->childrens[i], src->mChildren[i]);
	}
}

static void RecursiveFreeNodes(anim_node_t* node)
{
	for (uint32_t i = 0; i < node->childrenCount; i++) {
		RecursiveFreeNodes(&node->childrens[i]);
	}

	if (!node->childrens) return;

	Array_Destroy(node->childrens);
	node->childrens = NULL;
}

static void FindAnimBones(animation_t* animation, model_t* model, const struct aiAnimation* ai_anim)
{
	animation->bones = Array_CreateSize(anim_bone_t, ai_anim->mNumChannels);

	for (uint32_t i = 0; i < ai_anim->mNumChannels; i++) {
		const struct aiNodeAnim* channel = ai_anim->mChannels[i];

		bone_info_t* boneInfo = Model_FindBoneInfo(model, channel->mNodeName.data);	

		if (!boneInfo) {
			LOG_ERROR("Dawg, we got no bone called %s", channel->mNodeName.data);
			
			Array_Push(model->bones.infos, (bone_info_t) { });
			boneInfo = &model->bones.infos[model->bones.count];

			boneInfo->id = model->bones.count++;
			if (channel->mNodeName.length > 0) {
				strncpy(boneInfo->name, channel->mNodeName.data, MATH_MIN(channel->mNodeName.length, MAX_BONEINFO_NAME_LENGTH));
			} else {
				Sys_MemZero(boneInfo->name, MAX_BONEINFO_NAME_LENGTH);
			}
			
			boneInfo->offset = MAT4_IDENTITY;
		}

		AnimationBone_Init(&animation->bones[i], boneInfo, channel);
	}
}

bool Animation_Init(animation_t* animation, model_t* model, const void* scene, uint32_t index)
{
	const struct aiScene* aiscene = scene;
	const struct aiAnimation* aianim = aiscene->mAnimations[index];

	animation->duration = aianim->mDuration;
	animation->ticksPerSecond = aianim->mTicksPerSecond;

	RecursiveNodeData(animation, &animation->rootNode, aiscene->mRootNode);

	FindAnimBones(animation, model, aianim);

	return true;
}

bool Animation_InitFromPath(animation_t* animation, model_t* model, const char* path, uint32_t index)
{
	animation->bones = NULL;
	animation->duration = 0.0f;
	animation->ticksPerSecond = 0;

	if (!path) return false;

	const char* fileExtension = Sys_PathGetExtension(path);
	if (!fileExtension || aiIsExtensionSupported(fileExtension) == AI_FALSE) {
		LOG_ERROR("Model path does not have a valid or supported extension");
		return false;
	}
	
	const struct aiScene* scene = NULL;
	scene = aiImportFile(path, aiProcess_Triangulate);

	if (!scene) {
		LOG_ERROR("Model Animation could not be loaded");
		return false;
	}

	if (index >= scene->mNumAnimations || !scene->mAnimations[index]) {
		LOG_ERROR("Model Animation does not exist");
		return false;
	}

	const struct aiAnimation* anim = scene->mAnimations[index];

	animation->duration = anim->mDuration;
	animation->ticksPerSecond = anim->mTicksPerSecond;

	RecursiveNodeData(animation, &animation->rootNode, scene->mRootNode);
	
	LOG_INFO("-------OLD-------");
	for (uint32_t i = 0; i < model->bones.count; i++) {
		LOG_INFO("Bone \"%s\", %i", model->bones.infos[i].name, model->bones.infos[i].id);
	}
	LOG_INFO("---------------");
	FindAnimBones(animation, model, anim);
	LOG_INFO("-------NEW-------");
	for (uint32_t i = 0; i < model->bones.count; i++) {
		LOG_INFO("Bone \"%s\", %i", model->bones.infos[i].name, model->bones.infos[i].id);
	}
	LOG_INFO("---------------");

	aiReleaseImport(scene);

	return true;
}

anim_bone_t* Animation_FindBone(animation_t* animation, const char* name)
{
	uint32_t boneCount = (uint32_t)Array_Size(animation->bones);
	for (uint16_t i = 0; i < boneCount; i++) {
		anim_bone_t* bone = &animation->bones[i];
		if (strncmp(name, bone->boneInfo->name, MAX_BONEINFO_NAME_LENGTH) == 0) {
			return bone;
		}
	}

	return NULL;
}

void Animation_Destroy(animation_t* animation)
{
	uint32_t boneCount = (uint32_t)Array_Size(animation->bones);
	for (uint32_t i = 0; i < boneCount; i++) {
		AnimationBone_Destroy(&animation->bones[i]);
	}

	Array_Destroy(animation->bones);

	RecursiveFreeNodes(&animation->rootNode);
}

static void RecursiveCalcBoneTransform(animator_t* animator, const anim_node_t* node, const mat4_t parent_transform)
{
	anim_bone_t* bone = Animation_FindBone(animator->animation, node->name);
	
	mat4_t transform;
	mat4_t globalTransform = MAT4_IDENTITY;
	if (bone) {		
		AnimationBone_Update(bone, animator->time);
		transform = bone->localTrans;
	} else {
		transform = node->transform;
	}
	Mat4_Mul(&parent_transform, &transform, &globalTransform);
	
	if (bone) {
		Mat4_Mul(&globalTransform, &bone->boneInfo->offset, &animator->finalMatrices[bone->boneInfo->id]);
	}

	for (uint16_t i = 0; i < node->childrenCount; i++) {
		RecursiveCalcBoneTransform(animator, &node->childrens[i], globalTransform);
	}
}

void Animator_Init(animator_t* animator, animation_t* animation)
{
	animator->time = 0.0f;
	animator->frametime = 0.0f;
	animator->animation = animation;
	animator->finalMatrices = Array_CreateSize(mat4_t, 100);
}

void Animator_Update(animator_t* animator, float frametime)
{
	animator->frametime = frametime;

 	if (animator->animation) {
		animator->time += (float)animator->animation->ticksPerSecond * frametime;
		animator->time = fmodf(animator->time, animator->animation->duration);

		mat4_t parentTransform = MAT4_IDENTITY;
		RecursiveCalcBoneTransform(animator, &animator->animation->rootNode, parentTransform);
	}
}

void Animator_Destroy(animator_t* animator)
{
	if (animator->finalMatrices) {
		Array_Destroy(animator->finalMatrices);
	}
}
