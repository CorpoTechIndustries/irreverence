#include <engine/renderer/framebuffer.h>

#include <GL/glew.h>

void Framebuffer_Init(framebuffer_t* framebuffer, uint32_t width, uint32_t height)
{
	framebuffer->id = 0;
}

void Framebuffer_Destroy(framebuffer_t* framebuffer)
{
	glDeleteFramebuffers(1, &framebuffer->id);
	
	for (uint8_t i = 0; i < MAX_FRAMEBUFFER_COLOR; i++) {
		texture_t* texture = &framebuffer->colorTextures[i];
		if (texture->id == 0) continue;

		Texture_Destroy(texture);
	}

	if (framebuffer->rboId != 0) {
		glDeleteRenderbuffers(1, &framebuffer->rboId);
	}
	else {
		Texture_Destroy(&framebuffer->depthTexture); // If we don't have RBO then we must have Depth Texture
	}
}

static uint32_t s_iBoundFramebuffer = 0;
void Framebuffer_Bind(framebuffer_t* framebuffer)
{
	if (s_iBoundFramebuffer == framebuffer->id) return;
	s_iBoundFramebuffer = framebuffer->id;

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
	glViewport(0, 0, framebuffer->width, framebuffer->height);
}

void Framebuffer_UnBind()
{
	if (s_iBoundFramebuffer == 0) return;
	s_iBoundFramebuffer = 0;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
}