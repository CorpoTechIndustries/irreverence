#include <engine/renderer/mesh.h>

#include <stddef.h>

#include <GL/glew.h>

#define SetMatrixLayout(vertexarray, location, offset) for (unsigned int i = 0; i < 4; i++) { unsigned int mat_offset = location + i; glEnableVertexArrayAttrib(vertexarray, mat_offset); glVertexArrayAttribFormat(vertexarray, mat_offset, 4, GL_FLOAT, GL_FALSE, offset + sizeof(float) * 4 * i); glVertexArrayAttribBinding(vertexarray, mat_offset, 1); }

void Mesh_InitModel(mesh_t* mesh, mesh_modelvertex_t* vertices, uint32_t vertex_count, uint32_t* indices, uint32_t index_count)
{
	enum {
		MODELMESH_POSITION,
		MODELMESH_NORMAL,
		MODELMESH_UV,
		MODELMESH_COLOR,
		MODELMESH_MODELMATRIX
	};

	size_t vertexSize = vertex_count * sizeof(mesh_modelvertex_t);
	size_t indexSize = index_count * sizeof(uint32_t);

	uint32_t id = 0;
	uint32_t vbo = 0;
	uint32_t ibo = 0;
	uint32_t ebo = 0;

	// Create OpenGL Objects
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, vertexSize, vertices, GL_DYNAMIC_STORAGE_BIT);

	glDisable(GL_DEBUG_OUTPUT);
	glCreateBuffers(1, &ibo);
	glNamedBufferStorage(ibo, 0, NULL, GL_DYNAMIC_STORAGE_BIT);
	glEnable(GL_DEBUG_OUTPUT);

	glCreateBuffers(1, &ebo);
	glNamedBufferStorage(ebo, indexSize, indices, GL_DYNAMIC_STORAGE_BIT);

	glCreateVertexArrays(1, &id);

	// Link Element Buffer
	glVertexArrayElementBuffer(id, ebo);

	// Link Vertex Buffer and Set Vertex Buffer Layout  |	Position: 3 floats, Normals: 3 floats, UV: 2 floats
	glVertexArrayVertexBuffer(id, 0, vbo, 0, 8 * sizeof(float));

	glEnableVertexArrayAttrib(id, MODELMESH_POSITION);
	glVertexArrayAttribFormat(id, MODELMESH_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(id, MODELMESH_POSITION, 0);

	glEnableVertexArrayAttrib(id, MODELMESH_NORMAL);
	glVertexArrayAttribFormat(id, MODELMESH_NORMAL, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
	glVertexArrayAttribBinding(id, MODELMESH_NORMAL, 0);

	glEnableVertexArrayAttrib(id, MODELMESH_UV);
	glVertexArrayAttribFormat(id, MODELMESH_UV, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
	glVertexArrayAttribBinding(id, MODELMESH_UV, 0);

	// Link Instance Buffer and Set Instance Buffer Layout  |	Color: 4 floats, Model Matrix: 16 floats
	glVertexArrayVertexBuffer(id, 1, ibo, 0, 20 * sizeof(float));
	glVertexArrayBindingDivisor(id, 1, 1);

	glEnableVertexArrayAttrib(id, MODELMESH_COLOR);
	glVertexArrayAttribFormat(id, MODELMESH_COLOR, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(id, MODELMESH_COLOR, 1);

	SetMatrixLayout(id, MODELMESH_MODELMATRIX, 4 * sizeof(float));

	mesh->id = id;
	mesh->vbo = vbo;
	mesh->ibo = ibo;
	mesh->ebo = ebo;

	mesh->vertexCount = vertex_count;
	mesh->indexCount = index_count;
}

void Mesh_InitSkybox(mesh_t* mesh, mesh_skyvertex_t* vertices, uint32_t vertex_count, uint32_t* indices, uint32_t index_count)
{
	mesh->vertexCount = vertex_count;
	mesh->indexCount = index_count;

	size_t vertexSize = vertex_count * sizeof(mesh_skyvertex_t);
	size_t indexSize = index_count * sizeof(uint32_t);
}

void Mesh_Destroy(mesh_t* mesh)
{
	glDeleteVertexArrays(1, &mesh->id);
	mesh->id = 0;

	glDeleteBuffers(1, &mesh->vbo);
	glDeleteBuffers(1, &mesh->ebo);
	glDeleteBuffers(1, &mesh->ibo);
}

void Mesh_DrawModel(mesh_t* mesh, const mesh_modelinstance_t* instance)
{
	glNamedBufferData(mesh->ibo, sizeof(mesh_modelinstance_t), instance, GL_DYNAMIC_DRAW);

	glBindVertexArray(mesh->id);
	glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh_DrawSkybox(mesh_t* mesh)
{

}