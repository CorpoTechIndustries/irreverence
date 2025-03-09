#include <engine/renderer/uniform.h>

#include <GL/glew.h>

void Uniform_Init(uniform_t* uniform, uint8_t location, void* data, size_t data_size)
{
	uniform->id = 0;
	uniform->currentSize = data_size;
}

void Uniform_Destroy(uniform_t* uniform)
{
	glDeleteBuffers(1, &uniform->id);
	uniform->id = 0;
}

void Uniform_Update(uniform_t* uniform, void* data)
{

}

void Uniform_Relocate(uniform_t* uniform, uint8_t location)
{

}