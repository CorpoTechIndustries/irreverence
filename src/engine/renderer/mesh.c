#include <engine/renderer/mesh.h>

#include <engine/log.h>

#include <platform/memory.h>

#include <stddef.h>

#include <GL/glew.h>

#define SetMatrixLayout(vertexarray, location, offset) for (unsigned int i = 0; i < 4; i++) { unsigned int mat_offset = location + i; glEnableVertexArrayAttrib(vertexarray, mat_offset); glVertexArrayAttribFormat(vertexarray, mat_offset, 4, GL_FLOAT, GL_FALSE, offset + sizeof(float) * 4 * i); glVertexArrayAttribBinding(vertexarray, mat_offset, 1); }

bool Mesh_InitModel(mesh_t* mesh, const mesh_vertexmodel_t* vertices, uint32_t vertex_count, const uint32_t* indices, uint32_t index_count)
{
	enum {
		MODELMESH_POSITION,
		MODELMESH_NORMAL,
		MODELMESH_UV,
		MODELMESH_BONEIDS,
		MODELMESH_BONEWEIGHTS,
		MODELMESH_COLOR,
		MODELMESH_MODELMATRIX
	};

	mesh->vertexCount = vertex_count;
	mesh->indexCount = index_count;

	mesh->instances.array = NULL;
	mesh->instances.stride = sizeof(mesh_instancemodel_t);
	mesh->instances.count = 0;
	mesh->instances.capacity = 0;
	
	size_t vertexSize = vertex_count * sizeof(mesh_vertexmodel_t);
	size_t indexSize = index_count * sizeof(uint32_t);
		
	// Create OpenGL Objects
	glCreateBuffers(1, &mesh->vbo);
	glNamedBufferStorage(mesh->vbo, vertexSize, vertices, GL_DYNAMIC_STORAGE_BIT);

	glDisable(GL_DEBUG_OUTPUT);
	glCreateBuffers(1, &mesh->ibo);
	glNamedBufferStorage(mesh->ibo, 0, NULL, GL_DYNAMIC_STORAGE_BIT);
	glEnable(GL_DEBUG_OUTPUT);

	glCreateBuffers(1, &mesh->ebo);
	glNamedBufferStorage(mesh->ebo, indexSize, indices, GL_DYNAMIC_STORAGE_BIT);

	glCreateVertexArrays(1, &mesh->id);

	// Link Element Buffer
	glVertexArrayElementBuffer(mesh->id, mesh->ebo);

	// Link Vertex Buffer and Set Vertex Buffer Layout
	glVertexArrayVertexBuffer(mesh->id, 0, mesh->vbo, 0, sizeof(mesh_vertexmodel_t));

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_POSITION);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(mesh->id, MODELMESH_POSITION, 0);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_NORMAL);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_NORMAL, 3, GL_FLOAT, GL_FALSE, offsetof(mesh_vertexmodel_t, nx));
	glVertexArrayAttribBinding(mesh->id, MODELMESH_NORMAL, 0);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_UV);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_UV, 2, GL_FLOAT, GL_FALSE, offsetof(mesh_vertexmodel_t, tx));
	glVertexArrayAttribBinding(mesh->id, MODELMESH_UV, 0);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_BONEIDS);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_BONEIDS, 4, GL_INT, GL_FALSE, offsetof(mesh_vertexmodel_t, b1));
	glVertexArrayAttribBinding(mesh->id, MODELMESH_BONEIDS, 0);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_BONEWEIGHTS);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_BONEWEIGHTS, 4, GL_FLOAT, GL_FALSE, offsetof(mesh_vertexmodel_t, w1));
	glVertexArrayAttribBinding(mesh->id, MODELMESH_BONEWEIGHTS, 0);

	// Link Instance Buffer and Set Instance Buffer Layout
	glVertexArrayVertexBuffer(mesh->id, 1, mesh->ibo, 0, sizeof(mesh_instancemodel_t));
	glVertexArrayBindingDivisor(mesh->id, 1, 1);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_COLOR);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_COLOR, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(mesh->id, MODELMESH_COLOR, 1);

	SetMatrixLayout(mesh->id, MODELMESH_MODELMATRIX, offsetof(mesh_instancemodel_t, model));

	return true;
}

bool Mesh_InitSkybox(mesh_t* mesh, const mesh_vertexsky_t* vertices, uint32_t vertex_count)
{
	
	size_t vertexSize = vertex_count * sizeof(mesh_vertexsky_t);

	mesh->vertexCount = vertex_count;
	mesh->indexCount = 0;

	return true;
}

void Mesh_Destroy(mesh_t* mesh)
{
	glDeleteVertexArrays(1, &mesh->id);
	mesh->id = 0;

	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);
	glDeleteBuffers(1, &mesh->ibo);

	if (mesh->instances.array) {
		Sys_Free(mesh->instances.array);
		mesh->instances.array = NULL;
	}
}

void Mesh_AddInstance(mesh_t* mesh, const void* data)
{
	if (mesh->instances.stride == 0) {
		return;
	} 

	if (!mesh->instances.array) {
		mesh->instances.array = Sys_Calloc(mesh->instances.stride);
		mesh->instances.capacity = 1;
	}

	if (mesh->instances.count >= mesh->instances.capacity) {
		mesh->instances.capacity *= 2;
		mesh->instances.array = Sys_ReAlloc(mesh->instances.array, mesh->instances.capacity * mesh->instances.stride);
	}

	uint8_t* data_c = (uint8_t*)data;
	uint8_t* instance_c = mesh->instances.array + mesh->instances.count * mesh->instances.stride;
	for (uint32_t i = 0; i < mesh->instances.stride; i++) {
		instance_c[i] = data_c[i];
	}

	mesh->instances.count++;
}

void Mesh_ClearInstances(mesh_t* mesh)
{
	// Yea, this is all it does. We don't need to memset because the data will be overriden either way.
	mesh->instances.count = 0;
}

void Mesh_Draw(mesh_t* mesh, const void* data)
{
	glNamedBufferData(mesh->ibo, mesh->instances.stride, data, GL_DYNAMIC_DRAW);

	if (mesh->ebo) {
		glBindVertexArray(mesh->id);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	} else {
		glBindVertexArray(mesh->id);
		glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
		glBindVertexArray(0);
	}
}

void Mesh_DrawInstances(mesh_t* mesh)
{
	glNamedBufferData(mesh->ibo, mesh->instances.stride * mesh->instances.count, mesh->instances.array, GL_DYNAMIC_DRAW);

	if (mesh->ebo) {
		glBindVertexArray(mesh->id);
		glDrawElementsInstanced(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, NULL, mesh->instances.count);
		glBindVertexArray(0);
	} else {
		glBindVertexArray(mesh->id);
		glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->vertexCount, mesh->instances.count);
		glBindVertexArray(0);
	}
}
