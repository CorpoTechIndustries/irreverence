#include <engine/renderer/uniform.h>

#include <GL/glew.h>

bool Uniform_Init(uniform_t* uniform, uint8_t location, void* data, size_t size)
{
	uniform->location = location;
	uniform->currentSize = size;

	glCreateBuffers(1, &uniform->id);
	glNamedBufferData(uniform->id, (GLsizeiptr)size, data, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, location, uniform->id);

	return true;
}

void Uniform_Destroy(uniform_t* uniform)
{
	glDeleteBuffers(1, &uniform->id);
	uniform->id = 0;
}

void Uniform_Update(uniform_t* uniform, void* data, size_t data_size, size_t offset)
{
	if (data_size == 0 || data_size + offset > uniform->currentSize) return;

	glNamedBufferSubData(uniform->id, offset, data_size, data);
}

void Uniform_Bind(uniform_t* uniform)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, uniform->location, uniform->id);
}

void Uniform_Relocate(uniform_t* uniform, uint8_t location)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, location, uniform->id);
	uniform->location = location;
}
