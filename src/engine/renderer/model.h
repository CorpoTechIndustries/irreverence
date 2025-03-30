#pragma once

#include <engine/renderer/mesh.h>
#include <engine/renderer/material.h>

#include <util/array.h>

#define MAX_MODEL_SKINS 1
#define MAX_MODEL_BONES UINT16_MAX
#define MAX_MODEL_WEIGHT 4 
#define MAX_BONEINFO_NAME_LENGTH 32

typedef struct BoneInfo {
	char name[MAX_BONEINFO_NAME_LENGTH];
	mat4_t offset;
	int32_t id;
} bone_info_t;

typedef struct Model {
	mesh_t* meshes;
	material_t* meshMaterials;

	struct {
		bone_info_t* infos;
		uint32_t count;
	} bones;
} model_t;

bool Model_Init(model_t* model, const char* path);
bool Model_InitFromMeshes(model_t* model, mesh_t* meshes, uint32_t mesh_count);
void Model_Destroy(model_t* model);

void Model_AddInstance(model_t* model, const mesh_instancemodel_t* instance);
void Model_ClearInstances(model_t* model);

void Model_Draw(model_t* model, const mesh_instancemodel_t* instance, uint32_t skin);
void Model_DrawInstances(model_t* model);

bone_info_t* Model_FindBoneInfo(model_t* model, const char* name);
