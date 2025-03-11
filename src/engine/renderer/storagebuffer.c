#include <engine/renderer/storagebuffer.h>

#include <GL/glew.h>
#include "storagebuffer.h"

void Storagebuffer_Init(storagebuffer_t* storagebuffer, uint8_t location, void* data, size_t size, bool realloc)
{
	storagebuffer->id = 0;
	storagebuffer->realloc = realloc;
	storagebuffer->location = location;
	storagebuffer->currentSize = size;

	unsigned int id = 0;
	glCreateBuffers(1, &storagebuffer->id);
	glNamedBufferStorage(storagebuffer->id, size, data, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, storagebuffer->id);

	// TODO: if realloc is true, store a copy of the data and update that data every it is changed. It would be used for re-giving the data of reallocated buffer
}

void Storagebuffer_Destroy(storagebuffer_t* storagebuffer)
{
	glDeleteBuffers(1, &storagebuffer->id);
	storagebuffer->id = 0;
}

void Storagebuffer_Update(storagebuffer_t* storagebuffer, void* data, size_t data_size, size_t offset)
{
	if (data_size == 0) return;

	size_t size_offset = data_size + offset;

	bool overflow = size_offset > storagebuffer->currentSize;

	if (overflow) {
		if (!storagebuffer->realloc) return;
		else {
			glDeleteBuffers(1, &storagebuffer->id);

			while (size_offset > storagebuffer->currentSize) storagebuffer->currentSize *= 2; // Multiply till it is big enough.

			glCreateBuffers(1, &storagebuffer->id);
			glNamedBufferStorage(storagebuffer->id, storagebuffer->currentSize, NULL, GL_DYNAMIC_STORAGE_BIT); 
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, storagebuffer->location, storagebuffer->id);
		}
	}

	glNamedBufferSubData(storagebuffer->id, offset, (GLsizeiptr)data_size, data);
}