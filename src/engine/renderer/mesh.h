#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <math/mat4.h>

typedef struct {
	float x, y, z; 			// Position
	float nx, ny, nz;		// Normal
	float tx, ty;			// UV
	uint8_t b1, b2, b3, b4; // Bone Ids
	float w1, w2, w3, w4;	// Bone Weights
} mesh_vertexmodel_t;

typedef struct {
	float x, y, z; 		// Position
	float tx, ty, tz;	// UV
} mesh_vertexsky_t;

typedef struct {
	float r, g, b, a;	// Color
	mat4_t model;		// Model Matrix
} mesh_instancemodel_t;

typedef struct {
	uint32_t id;
	uint32_t vbo;
	uint32_t ibo;
	uint32_t ebo;
	uint32_t vertexCount;
	uint32_t indexCount;
	
	struct {
		uint32_t stride;
		uint32_t count;
		uint32_t capacity;
		uint8_t* array;
	} instances;
} mesh_t;

bool Mesh_InitModel(mesh_t* mesh, const mesh_vertexmodel_t* vertices, uint32_t vertex_count, const uint32_t* indices, uint32_t index_count);
bool Mesh_InitSkybox(mesh_t* mesh, const mesh_vertexsky_t* vertices, uint32_t vertex_count);
void Mesh_Destroy(mesh_t* mesh);

void Mesh_AddInstance(mesh_t* mesh, const void* data);
void Mesh_ClearInstances(mesh_t* mesh);

void Mesh_Draw(mesh_t* mesh, const void* data);
void Mesh_DrawInstances(mesh_t* mesh);
