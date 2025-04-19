#include <engine/renderer/storagebuffer.h>

#include <platform/memory.h>

#include <GL/glew.h>

bool Storagebuffer_Init(storagebuffer_t* storagebuffer, uint8_t location, void* data, size_t size, bool realloc)
{
	storagebuffer->realloc = realloc;
	storagebuffer->location = location;
	storagebuffer->currentSize = size;
	storagebuffer->map = NULL;

	uint32_t flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;

	glCreateBuffers(1, &storagebuffer->id);
	glNamedBufferStorage(storagebuffer->id, size, data, flags);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, storagebuffer->id);

	storagebuffer->map = glMapNamedBufferRange(storagebuffer->id, 0, storagebuffer->currentSize, flags);

	return true;
}

void Storagebuffer_Destroy(storagebuffer_t* storagebuffer)
{
	if (storagebuffer->map) {
		glUnmapNamedBuffer(storagebuffer->id);
	}

	glDeleteBuffers(1, &storagebuffer->id);
	storagebuffer->id = 0;
}

void Storagebuffer_Update(storagebuffer_t* storagebuffer, void* data, size_t data_size, size_t offset)
{
	if (data_size == 0) return;

	size_t oldSize = storagebuffer->currentSize;

	size_t size_offset = data_size + offset;

	bool overflow = size_offset > storagebuffer->currentSize;

	if (overflow) {
		if (!storagebuffer->realloc) return;

		uint32_t newId = 0;
		
		while (size_offset > storagebuffer->currentSize) storagebuffer->currentSize *= 2; // Multiply till it is big enough.
		
		uint32_t flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;

		glCreateBuffers(1, &newId);
		glNamedBufferStorage(newId, storagebuffer->currentSize, NULL, flags);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, storagebuffer->location, newId);
		
		glCopyNamedBufferSubData(storagebuffer->id, newId, 0, 0, oldSize);

		storagebuffer->map = glMapNamedBufferRange(newId, 0, storagebuffer->currentSize, flags);

		glUnmapNamedBuffer(storagebuffer->id);
		glDeleteBuffers(1, &storagebuffer->id);

		storagebuffer->id = newId;
	}

	Sys_MemCpy(data, storagebuffer->map + offset, data_size);
	glFlushMappedBufferRange(storagebuffer->id, offset, data_size);
}
