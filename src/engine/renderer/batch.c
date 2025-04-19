#include <engine/renderer/batch.h>

#include <engine/log.h>

#include <util/array.h>

#include <platform/memory.h>

#include <GL/glew.h>

bool Batch_Init(batch_t* batch, const mesh_attribute_t* attributes, uint8_t attribute_count, bool use_indices)
{
	batch->id = 0;
	batch->vbo = 0;
	batch->ebo = 0;
	batch->indexArr = NULL;
	batch->dirty = true;
	
	batch->vertices.capacity = 0;
	batch->vertices.count = 0;
	batch->vertices.stride = 0;
	batch->vertices.data = 0;

	batch->beginVertex = NULL;
	batch->beginVertexOff = 0;

	glCreateVertexArrays(1, &batch->id);
	
	if (use_indices) {
		glCreateBuffers(1, &batch->ebo);
		glVertexArrayElementBuffer(batch->id, batch->ebo);

		batch->indexArr = Array_Create(uint32_t);
	}
	
	if (attributes && attribute_count > 0) {
		glCreateBuffers(1, &batch->vbo);

		size_t vertAttTotalSize = 0;
		size_t* vertAttSizes = Sys_Malloc(sizeof(size_t) * attribute_count); 
		for (size_t i = 0; i < attribute_count; i++) {
			const mesh_attribute_t* attrib = &attributes[i];
			
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

		// Link Vertex Buffer and Set Vertex Buffer Layout
		glVertexArrayVertexBuffer(batch->id, 0, batch->vbo, 0, vertAttTotalSize);
		
		batch->vertices.stride = vertAttTotalSize;

		batch->beginVertex = Sys_Calloc(vertAttTotalSize);

		size_t vertAttribOffset = 0;
		for (uint8_t i = 0; i < attribute_count; i++) {
			const mesh_attribute_t* attrib = &attributes[i];
	
			glEnableVertexArrayAttrib(batch->id, i);
			glVertexArrayAttribFormat(batch->id, i, attrib->count, attrib->type, GL_FALSE, (GLuint)vertAttribOffset);
			glVertexArrayAttribBinding(batch->id, i, 0);
	
			vertAttribOffset += vertAttSizes[i];
		}
		
		Sys_Free(vertAttSizes);
	}

	return true;
}

void Batch_Destroy(batch_t* batch)
{
	glDeleteVertexArrays(1, &batch->id);
	batch->id = 0;

	glDeleteBuffers(1, &batch->vbo);
	glDeleteBuffers(1, &batch->ebo);

	if (batch->vertices.data) {
		Sys_Free(batch->vertices.data);
		batch->vertices.data = NULL;
	}

	batch->vertices.count = 0;

	if (batch->beginVertex) {
		Sys_Free(batch->beginVertex);
		batch->beginVertex = NULL;
	}

	if (batch->indexArr) {
		Array_Destroy(batch->indexArr);
		batch->indexArr = NULL;
	}
}

void Batch_PushVertex(batch_t* batch, const void* data)
{
	struct batch_vertices* vertices = &batch->vertices;

	if (!vertices->data) {
		vertices->data = Sys_Calloc(vertices->stride);
		vertices->capacity = 1;
	}

	if (vertices->count >= vertices->capacity) {
		vertices->capacity *= 2;
		vertices->data = Sys_ReAlloc(vertices->data, vertices->capacity * vertices->stride);
	}

	Sys_MemCpy(data, vertices->data + vertices->count * vertices->stride, vertices->stride);

	vertices->count++;

	batch->dirty = true;
}

void Batch_PushIndex(batch_t* batch, uint32_t index)
{
	if (!batch->ebo) {
		return;
	}

	Array_Push(batch->indexArr, index);
	batch->dirty = true;
}

void Batch_PushIndices(batch_t* batch, const uint32_t* indices, uint32_t count)
{
	if (!batch->ebo) {
		return;
	}

	uint32_t indCount = Array_Count(batch->indexArr);
	for (uint32_t i = 0; i < count; i++) {
		Array_Push(batch->indexArr, indCount + indices[i]);
	}
	
	batch->dirty = true;
}

void Batch_BeginVertex(batch_t* batch)
{
	batch->beginVertexOff = 0;
}

void Batch_VertexData(batch_t* batch, const void* data, size_t size)
{
	if (batch->beginVertexOff + size > batch->vertices.stride) {
		return;
	}

	Sys_MemCpy(data, batch->beginVertex + batch->beginVertexOff, size);
	batch->beginVertexOff += size;
}

void Batch_EndVertex(batch_t* batch)
{
	batch->beginVertexOff = 0;
	Batch_PushVertex(batch, batch->beginVertex);
}

void Batch_Draw(batch_t* batch)
{
	size_t vertexCount = batch->vertices.count;
	
	if (vertexCount <= 0) {
		return;
	}
	
	if (batch->dirty) {
		size_t vertexStride = batch->vertices.stride;
		glNamedBufferData(batch->vbo, vertexStride * vertexCount, batch->vertices.data, GL_DYNAMIC_DRAW);
	}
	
	glBindVertexArray(batch->id);
	
	if (batch->ebo > 0) {
		size_t indexCount = Array_Count(batch->indexArr);
		if (batch->dirty) {
			size_t elementStride = Array_Stride(batch->indexArr);
			glNamedBufferData(batch->ebo, elementStride * indexCount, batch->indexArr, GL_DYNAMIC_DRAW);
		}
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);
	} else {
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}

	batch->dirty = false;
}

void Batch_Clear(batch_t* batch)
{
	batch->vertices.count = 0;

	if (batch->indexArr) {
		Array_Clear(batch->indexArr);
	}

	batch->dirty = false;
}
