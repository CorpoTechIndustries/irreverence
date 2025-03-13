#pragma once

#include <stdint.h>

#include <math/mat4.h>

typedef struct {
	float x, y, z; 		// Position
	float nx, ny, nz;	// Normal
	float tx, ty;		// UV
} mesh_modelvertex_t;

typedef struct {
	float x, y, z; 		// Position
	float tx, ty, tz;	// UV
} mesh_skyvertex_t;

typedef struct {
	float r, g, b, a;	// Color
	mat4_t model;
} mesh_modelinstance_t;

typedef struct {
	uint32_t id;
	uint32_t vbo;
	uint32_t ibo;
	uint32_t ebo;
	uint32_t vertexCount;
	uint32_t indexCount;
} mesh_t;

void Mesh_InitModel(mesh_t* mesh, const mesh_modelvertex_t* vertices, uint32_t vertex_count, const uint32_t* indices, uint32_t index_count);
void Mesh_InitSkybox(mesh_t* mesh, const mesh_skyvertex_t* vertices, uint32_t vertex_count);
void Mesh_Destroy(mesh_t* mesh);

void Mesh_DrawModel(mesh_t* mesh, const mesh_modelinstance_t* instance);
void Mesh_DrawSkybox(mesh_t* mesh);