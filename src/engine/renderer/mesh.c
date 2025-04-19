#include <engine/renderer/mesh.h>

#include <engine/log.h>

#include <platform/memory.h>

#include <stddef.h>

#include <GL/glew.h>

#define SetMatrixLayout(vertexarray, location, offset) for (unsigned int i = 0; i < 4; i++) { unsigned int mat_offset = location + i; glEnableVertexArrayAttrib(vertexarray, mat_offset); glVertexArrayAttribFormat(vertexarray, mat_offset, 4, GL_FLOAT, GL_FALSE, offset + sizeof(float) * 4 * i); glVertexArrayAttribBinding(vertexarray, mat_offset, 1); }

struct mesh_instlist {
	uint32_t count;
	uint32_t capacity;
	uint8_t* array;
};

struct material_instlist_node {
	uint32_t key; // Material Id
	struct mesh_instlist list;
	struct material_instlist_node* next;
};

static struct material_instlist_node* FindMaterialInstList(mesh_t* mesh, uint32_t key)
{
	struct material_instlist_node* node = mesh->instListHead;

	if (!node) {
		return NULL;
	}

	do {
		if (node->key == key) {
			return node;
		}
		
	} while (node = node->next);

	return NULL;
}

static struct material_instlist_node* MakeMaterialInstList(mesh_t* mesh, uint32_t key)
{
	struct material_instlist_node* node = mesh->instListHead;

	if (!node) {
		node = Sys_Malloc(sizeof(struct material_instlist_node));
		node->key = key;
		node->list = (struct mesh_instlist) { 0 };
		node->next = NULL;

		mesh->instListHead = node;

		return node;
	}

	while (node->next) {
		node = node->next;
	}

	node->next = Sys_Malloc(sizeof(struct material_instlist_node));
	node->next->key = key;
	node->next->list = (struct mesh_instlist) { 0 };
	node->next->next = NULL;

	return node->next;
}

bool Mesh_InitCustom(
	mesh_t* mesh, 
	const void* vertices, 
	size_t vertices_size, 
	const uint32_t* indices,
	uint32_t index_count,
	const mesh_attribute_t* vert_attribs,
	uint8_t vert_attrib_count,
	const mesh_attribute_t* inst_attribs,
	uint8_t inst_attrib_count)
{
	mesh->vertexCount = 0;
	mesh->indexCount = 0;

	mesh->instStride = 0;
	mesh->instListHead = NULL;

	mesh->id = 0;
	mesh->vbo = 0;
	mesh->ebo = 0; 
	mesh->ibo = 0;

	glCreateVertexArrays(1, &mesh->id);
	
	if (indices && index_count > 0) {
		mesh->indexCount = index_count;
		size_t indexSize = index_count * sizeof(uint32_t);

		glCreateBuffers(1, &mesh->ebo);
		glNamedBufferStorage(mesh->ebo, indexSize, indices, GL_DYNAMIC_STORAGE_BIT);
	
		glVertexArrayElementBuffer(mesh->id, mesh->ebo);
	}
	
	if (vert_attribs && vert_attrib_count > 0) {
		glCreateBuffers(1, &mesh->vbo);
		glNamedBufferStorage(mesh->vbo, vertices_size, vertices, GL_DYNAMIC_STORAGE_BIT);

		size_t vertAttTotalSize = 0;
		size_t* vertAttSizes = Sys_Malloc(sizeof(size_t) * vert_attrib_count); 
		for (size_t i = 0; i < vert_attrib_count; i++) {
			const mesh_attribute_t* attrib = &vert_attribs[i];
			
			if (attrib->count == 0) {
				Sys_Free(vertAttSizes);
				// TODO: Assert
			}
	
			size_t attribSize = 0;
			switch (attrib->type)
			{
			case GL_FLOAT:
			case GL_INT:
			case GL_UNSIGNED_INT:
				attribSize = 4 * attrib->count;
				break;
	
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
				attribSize = 2 * attrib->count;
				break;
	
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
				attribSize = attrib->count;
				break;
	
			default:
				break;
			}
	
			vertAttTotalSize += attribSize;
	
			vertAttSizes[i] = attribSize;
		}
		
		mesh->vertexCount = vertices_size / vertAttTotalSize;
	
		// Link Vertex Buffer and Set Vertex Buffer Layout
		glVertexArrayVertexBuffer(mesh->id, 0, mesh->vbo, 0, vertAttTotalSize);
	
		size_t vertAttribOffset = 0;
		for (uint8_t i = 0; i < vert_attrib_count; i++) {
			const mesh_attribute_t* attrib = &vert_attribs[i];
	
			glEnableVertexArrayAttrib(mesh->id, i);
			glVertexArrayAttribFormat(mesh->id, i, attrib->count, attrib->type, GL_FALSE, (GLuint)vertAttribOffset);
			glVertexArrayAttribBinding(mesh->id, i, 0);
	
			vertAttribOffset += vertAttSizes[i];
		}
		
		Sys_Free(vertAttSizes);
	}

	if (inst_attribs && inst_attrib_count > 0) {
		glCreateBuffers(1, &mesh->ibo);

		size_t instAttTotalSize = 0;
		size_t* instAttSizes = Sys_Malloc(sizeof(size_t) * inst_attrib_count); 
		for (size_t i = 0; i < inst_attrib_count; i++) {
			const mesh_attribute_t* attrib = &inst_attribs[i];
			
			if (attrib->count == 0) {
				Sys_Free(instAttSizes);
				// TODO: Assert
			}
	
			size_t attribSize = 0;
			switch (attrib->type)
			{
			case GL_FLOAT:
			case GL_INT:
			case GL_UNSIGNED_INT:
				attribSize = 4 * attrib->count;
				break;
	
			case GL_SHORT:
			case GL_UNSIGNED_SHORT:
				attribSize = 2 * attrib->count;
				break;
	
			case GL_BYTE:
			case GL_UNSIGNED_BYTE:
				attribSize = attrib->count;
				break;
	
			default:
				break;
			}
	
			instAttTotalSize += attribSize;
	
			instAttSizes[i] = attribSize;
		}

		mesh->instStride = instAttTotalSize;

		glVertexArrayVertexBuffer(mesh->id, 1, mesh->ibo, 0, (GLsizei)instAttTotalSize);
		glVertexArrayBindingDivisor(mesh->id, 1, 1);

		size_t instAttribOffset = 0;
		for (uint8_t i = 0; i < inst_attrib_count; i++) {
			const mesh_attribute_t* attrib = &inst_attribs[i];
	
			uint8_t loc = i + vert_attrib_count;
			glEnableVertexArrayAttrib(mesh->id, loc);
			glVertexArrayAttribFormat(mesh->id, loc, attrib->count, attrib->type, GL_FALSE, (GLuint)instAttribOffset);
			glVertexArrayAttribBinding(mesh->id, loc, 1);
	
			instAttribOffset += instAttSizes[i];
		}


		Sys_Free(instAttSizes);
	}
}

bool Mesh_InitModel(mesh_t* mesh, const mesh_vertexmodel_t* vertices, uint32_t vertex_count, const uint32_t* indices, uint32_t index_count)
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

	mesh->instStride = sizeof(mesh_instancemodel_t);
	mesh->instListHead = NULL;

	size_t vertexSize = vertex_count * sizeof(mesh_vertexmodel_t);
	size_t indexSize = index_count * sizeof(uint32_t);

	// Create OpenGL Objects
	glCreateBuffers(1, &mesh->vbo);
	glNamedBufferStorage(mesh->vbo, vertexSize, vertices, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &mesh->ibo);

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

	// Link Instance Buffer and Set Instance Buffer Layout
	glVertexArrayVertexBuffer(mesh->id, 1, mesh->ibo, 0, sizeof(mesh_instancemodel_t));
	glVertexArrayBindingDivisor(mesh->id, 1, 1);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_COLOR);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_COLOR, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(mesh->id, MODELMESH_COLOR, 1);

	SetMatrixLayout(mesh->id, MODELMESH_MODELMATRIX, offsetof(mesh_instancemodel_t, model));

	return true;
}

bool Mesh_InitAnimated(mesh_t* mesh, const mesh_vertexanimated_t* vertices, uint32_t vertex_count, const uint32_t* indices, uint32_t index_count)
{
	enum {
		MODELMESH_POSITION,
		MODELMESH_NORMAL,
		MODELMESH_UV,
		MODELMESH_BONES,
		MODELMESH_WEIGHTS,
		MODELMESH_COLOR,
		MODELMESH_MODELMATRIX
	};

	mesh->vertexCount = vertex_count;
	mesh->indexCount = index_count;

	mesh->instStride = sizeof(mesh_instancemodel_t);
	mesh->instListHead = NULL;

	size_t vertexSize = vertex_count * sizeof(mesh_vertexanimated_t);
	size_t indexSize = index_count * sizeof(uint32_t);

	// Create OpenGL Objects
	glCreateBuffers(1, &mesh->vbo);
	glNamedBufferStorage(mesh->vbo, vertexSize, vertices, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &mesh->ibo);

	glCreateBuffers(1, &mesh->ebo);
	glNamedBufferStorage(mesh->ebo, indexSize, indices, GL_DYNAMIC_STORAGE_BIT);

	glCreateVertexArrays(1, &mesh->id);

	// Link Element Buffer
	glVertexArrayElementBuffer(mesh->id, mesh->ebo);

	// Link Vertex Buffer and Set Vertex Buffer Layout
	glVertexArrayVertexBuffer(mesh->id, 0, mesh->vbo, 0, sizeof(mesh_vertexanimated_t));

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_POSITION);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(mesh->id, MODELMESH_POSITION, 0);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_NORMAL);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_NORMAL, 3, GL_FLOAT, GL_FALSE, offsetof(mesh_vertexanimated_t, nx));
	glVertexArrayAttribBinding(mesh->id, MODELMESH_NORMAL, 0);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_UV);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_UV, 2, GL_FLOAT, GL_FALSE, offsetof(mesh_vertexanimated_t, tx));
	glVertexArrayAttribBinding(mesh->id, MODELMESH_UV, 0);

	glEnableVertexArrayAttrib(mesh->id, MODELMESH_BONES);
	glVertexArrayAttribIFormat(mesh->id, MODELMESH_BONES, 1, GL_UNSIGNED_INT, offsetof(mesh_vertexanimated_t, bones));
	glVertexArrayAttribBinding(mesh->id, MODELMESH_BONES, 0);
	
	glEnableVertexArrayAttrib(mesh->id, MODELMESH_WEIGHTS);
	glVertexArrayAttribFormat(mesh->id, MODELMESH_WEIGHTS, 4, GL_FLOAT, GL_FALSE, offsetof(mesh_vertexanimated_t, weights));
	glVertexArrayAttribBinding(mesh->id, MODELMESH_WEIGHTS, 0);

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


	struct material_instlist_node* instlist_node = mesh->instListHead;

	if (instlist_node) {
		struct material_instlist_node* instlist_next = NULL;

		do {
			instlist_next = instlist_node->next;

			Sys_Free(instlist_node->list.array);
			Sys_Free(instlist_node);
		} while (instlist_node = instlist_next);
	}
}

void Mesh_AddInstance(mesh_t* mesh, const void* data, material_t* material)
{
	if (mesh->instStride == 0) {
		return;
	}

	struct material_instlist_node* instlist_node = FindMaterialInstList(mesh, material->id);

	if (!instlist_node) {
		instlist_node = MakeMaterialInstList(mesh, material->id);
	}

	struct mesh_instlist* instlist = &instlist_node->list;

	if (!instlist->array) {
		instlist->array = Sys_Calloc(mesh->instStride);
		instlist->capacity = 1;
	}

	if (instlist->count >= instlist->capacity) {
		instlist->capacity *= 2;
		instlist->array = Sys_ReAlloc(instlist->array, instlist->capacity * mesh->instStride);
	}

	Sys_MemCpy(data, instlist->array + instlist->count * mesh->instStride, mesh->instStride);

	instlist->count++;
}

void Mesh_ClearInstances(mesh_t* mesh, material_t* material)
{
	// Yea, this is all it does. We don't need to memset because the data will be overriden either way.
	struct material_instlist_node* instlist_node = FindMaterialInstList(mesh, material->id);
	instlist_node->list.count = 0;
}

void Mesh_Draw(mesh_t* mesh, const void* data)
{
	glNamedBufferData(mesh->ibo, mesh->instStride, data, GL_DYNAMIC_DRAW);

	glBindVertexArray(mesh->id);
	if (mesh->ebo > 0) {
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, mesh->vertexCount);
	}
}

void Mesh_DrawInstances(mesh_t* mesh, material_t* material)
{
	struct material_instlist_node* instlist_node = FindMaterialInstList(mesh, material->id);
	struct mesh_instlist* instlist = &instlist_node->list;

	glNamedBufferData(mesh->ibo, mesh->instStride * instlist->count, instlist->array, GL_DYNAMIC_DRAW);

	glBindVertexArray(mesh->id);
	if (mesh->ebo > 0) {
		glDrawElementsInstanced(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, NULL, instlist->count);
	} else {
		glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->vertexCount, instlist->count);
	}
}
