#pragma once

#include <engine/renderer/mesh.h>
#include <engine/renderer/material.h>

#include <math/quat.h>
#include <math/aabb.h>

#include <util/array.h>

#define MAX_MODEL_SKINS 1
#define MAX_MODEL_BONES UINT8_MAX
#define MAX_MODEL_WEIGHTS 4
#define MAX_BONEINFO_NAME_LENGTH 32

typedef struct bone_info {
	dualquat_t offset;
	char name[MAX_BONEINFO_NAME_LENGTH];
	uint8_t id;
} bone_info_t;

typedef struct model {
	mesh_t* meshes;
	material_t* meshMaterials;
	
	struct {
		bone_info_t* list;
		uint8_t count;
	} bones;

	aabb_t aabb;
} model_t;

bool Model_Init(model_t* model, const char* path, bool animated);
bool Model_InitFromMeshes(model_t* model, mesh_t* meshes, uint32_t mesh_count);
void Model_Destroy(model_t* model);

void Model_AddInstance(model_t* model, const mesh_instancemodel_t* instance);
void Model_ClearInstances(model_t* model);

void Model_Draw(model_t* model, const mesh_instancemodel_t* instance, uint32_t skin);
void Model_DrawInstances(model_t* model);

bone_info_t* Model_FindBoneInfo(model_t* model, const char* name);
