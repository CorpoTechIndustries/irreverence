#pragma once

#include <engine/renderer/mesh.h>

#include <math/vec2.h>
#include <math/vec3.h>
#include <math/vec4.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint32_t id;
	uint32_t vbo;
	uint32_t ebo;
	bool dirty;
	struct batch_vertices {
		void* data;
		size_t stride;
		uint32_t count;
		uint32_t capacity;
	} vertices;

	void* beginVertex;
	uint32_t beginVertexOff;

	uint32_t* indexArr;
} batch_t;

bool Batch_Init(batch_t* batch, const mesh_attribute_t* attributes, uint8_t attribute_count, bool use_indices);
void Batch_Destroy(batch_t* batch);

void Batch_PushVertex(batch_t* batch, const void* data);
void Batch_PushIndex(batch_t* batch, uint32_t index);
void Batch_PushIndices(batch_t* batch, const uint32_t* indices, uint32_t count);

void Batch_BeginVertex(batch_t* batch);
void Batch_VertexData(batch_t* batch, const void* data, size_t size);
#define Batch_Vertex(__batch, __v) do { typeof(__v) __var = __v; Batch_VertexData(__batch, &__var, sizeof(__var)); } while(0)
void Batch_EndVertex(batch_t* batch);

void Batch_Draw(batch_t* batch);
void Batch_Clear(batch_t* batch);
