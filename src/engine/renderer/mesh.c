#include <engine/renderer/mesh.h>

#include <engine/log.h>

#include <stddef.h>

#include <GL/glew.h>

#define SetMatrixLayout(vertexarray, location, offset) for (unsigned int i = 0; i < 4; i++) { unsigned int mat_offset = location + i; glEnableVertexArrayAttrib(vertexarray, mat_offset); glVertexArrayAttribFormat(vertexarray, mat_offset, 4, GL_FLOAT, GL_FALSE, offset + sizeof(float) * 4 * i); glVertexArrayAttribBinding(vertexarray, mat_offset, 1); }

void Mesh_InitModel(mesh_t* mesh, const mesh_vertexmodel_t* vertices, uint32_t vertex_count, const uint32_t* indices, uint32_t index_count)
{
	enum {
		MODELMESH_POSITION,
		MODELMESH_NORMAL,
		MODELMESH_UV,
		MODELMESH_COLOR,
		MODELMESH_MODELMATRIX
	};

	mesh->vertexCount = vertex_count;
	mesh->indexCount = index_count;
	
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

	// Link Vertex Buffer and Set Vertex Buffer Layout  |	Position: 3 floats, Normals: 3 floats, UV: 2 floats
	glVertexArrayVertexBuffer(mesh->id, 0, mesh->vbo, 0, 8 * sizeof(float));

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_POSITION);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(mesh->id, MODELMESH_POSITION, 0);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_NORMAL);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_NORMAL, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
	glVertexArrayAttribBinding(mesh->id, MODELMESH_NORMAL, 0);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_UV);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_UV, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
	glVertexArrayAttribBinding(mesh->id, MODELMESH_UV, 0);

	// Link Instance Buffer and Set Instance Buffer Layout  |	Color: 4 floats, Model Matrix: 16 floats
	glVertexArrayVertexBuffer(mesh->id, 1, mesh->ibo, 0, 20 * sizeof(float));
	glVertexArrayBindingDivisor(mesh->id, 1, 1);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_COLOR);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_COLOR, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(mesh->id, MODELMESH_COLOR, 1);

	SetMatrixLayout(mesh->id, MODELMESH_MODELMATRIX, 4 * sizeof(float));
}

void Mesh_InitSkybox(mesh_t* mesh, const mesh_vertexsky_t* vertices, uint32_t vertex_count)
{
	
	size_t vertexSize = vertex_count * sizeof(mesh_vertexsky_t);

	mesh->vertexCount = vertex_count;
	mesh->indexCount = 0;
}

void Mesh_Destroy(mesh_t* mesh)
{
	glDeleteVertexArrays(1, &mesh->id);
	mesh->id = 0;

	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);
	glDeleteBuffers(1, &mesh->ibo);
}

void Mesh_DrawModel(mesh_t* mesh, const mesh_instancemodel_t* instance)
{
	glNamedBufferData(mesh->ibo, sizeof(mesh_instancemodel_t), instance, GL_DYNAMIC_DRAW);

	glBindVertexArray(mesh->id);
	glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh_DrawSkybox(mesh_t* mesh)
{
	glBindVertexArray(mesh->id);
	glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
	glBindVertexArray(0);
}
