#pragma once

#include <engine/renderer/mesh.h>
#include <engine/renderer/material.h>

#include <util/array.h>

#define MAX_MODEL_SKINS 1

typedef struct {
	mesh_t* meshes;
	material_t* meshMaterials;
} model_t;

void Model_Init(model_t* model, const char* path);
void Model_InitFromMeshes(model_t* model, mesh_t* meshes, uint32_t mesh_count);
void Model_Destroy(model_t* model);

void Model_Draw(model_t* model, const mesh_instancemodel_t* instance, uint32_t skin);
