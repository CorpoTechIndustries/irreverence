#pragma once

#include <engine/renderer/material.h>

#include <math/mat4.h>

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
	float x, y, z; 			// Position
	float nx, ny, nz;		// Normal
	float tx, ty;			// UV
} mesh_vertexmodel_t;

typedef struct {
	float x, y, z; 			// Position
	float nx, ny, nz;		// Normal
	float tx, ty;			// UV
	uint8_t bones[4];
	float weights[4];
} mesh_vertexanimated_t;

typedef struct {
	float x, y, z; 		// Position
	float tx, ty, tz;	// UV
} mesh_vertexsky_t;

typedef struct {
	float r, g, b, a;	// Color
	mat4_t model;		// Model Matrix
} mesh_instancemodel_t;

typedef struct {
	uint32_t count;
	uint32_t capacity;
	uint8_t* array;
} mesh_instlist_t;

typedef struct {
	uint32_t id;
	uint32_t vbo;
	uint32_t ibo;
	uint32_t ebo;
	uint32_t vertexCount;
	uint32_t indexCount;

	size_t instStride;
	struct material_instlist_node* instListHead;
} mesh_t;

bool Mesh_InitModel(mesh_t* mesh, const mesh_vertexmodel_t* vertices, uint32_t vertex_count, const uint32_t* indices, uint32_t index_count);
bool Mesh_InitAnimated(mesh_t* mesh, const mesh_vertexanimated_t* vertices, uint32_t vertex_count, const uint32_t* indices, uint32_t index_count);
bool Mesh_InitSkybox(mesh_t* mesh, const mesh_vertexsky_t* vertices, uint32_t vertex_count);
void Mesh_Destroy(mesh_t* mesh);

void Mesh_AddInstance(mesh_t* mesh, const void* data, material_t* material);
void Mesh_ClearInstances(mesh_t* mesh, material_t* material);

void Mesh_Draw(mesh_t* mesh, const void* data);
void Mesh_DrawInstances(mesh_t* mesh, material_t* material);
