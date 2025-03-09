#include <engine/renderer/storagebuffer.h>

#include <GL/glew.h>
#include "storagebuffer.h"

void Storagebuffer_Init(storagebuffer_t* storagebuffer, void* data, size_t size)
{
	storagebuffer->id = 0;
	storagebuffer->currentSize = size;
}

void Storagebuffer_Destroy(storagebuffer_t* storagebuffer)
{
	glDeleteBuffers(1, &storagebuffer->id);
}

void Storagebuffer_Update(storagebuffer_t* storagebuffer, void* data)
{
	
}