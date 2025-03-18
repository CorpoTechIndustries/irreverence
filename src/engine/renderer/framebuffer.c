#include <engine/renderer/framebuffer.h>

#include <engine/renderer/renderer.h>
#include <engine/log.h>

#include <platform/memory.h>

#include <GL/glew.h>

bool Framebuffer_Init(
	framebuffer_t* framebuffer, 
	uint32_t width, 
	uint32_t height, 
	uint8_t samples,
	framebuffer_attachment_t* attachments, 
	uint8_t attachment_count,
	framebuffer_attachment_t* depth_attachment)
{
	framebuffer->id = 0;
	framebuffer->rbo = 0;
	framebuffer->width = width;
	framebuffer->height = height;
	Sys_MemZero(&framebuffer->depthTexture, sizeof(framebuffer->depthTexture));
	Sys_MemZero(framebuffer->colorTextures, sizeof(framebuffer->colorTextures));

	if (attachment_count > MAX_FRAMEBUFFER_COLOR) {
		return false;
	}

	glCreateFramebuffers(1, &framebuffer->id);
	
	for (uint8_t i = 0; i < attachment_count; i++) {
		framebuffer_attachment_t attachment = attachments[i];
		texture_t* colorTexture = &framebuffer->colorTextures[i];
		
		Texture_InitColorAttachment(colorTexture, framebuffer, i, width, height, samples, attachment.format, attachment.type);
	}

	if (depth_attachment) {
		Texture_InitDepthAttachment(&framebuffer->depthTexture, framebuffer, width, height, samples, depth_attachment->format, depth_attachment->type);
	} else {
		glCreateRenderbuffers(1, &framebuffer->rbo);

		if (samples == 0) glNamedRenderbufferStorage(framebuffer->rbo, GL_DEPTH24_STENCIL8, width, height);
		else glNamedRenderbufferStorageMultisample(framebuffer->rbo, samples, GL_DEPTH24_STENCIL8, width, height);

		glNamedFramebufferRenderbuffer(framebuffer->id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer->rbo);
	}

	if (glCheckNamedFramebufferStatus(framebuffer->id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("Couldn't create a Framebuffer. Something fucked up.");
		glDeleteFramebuffers(1, &framebuffer->id);

		if (framebuffer->rbo) {
			glDeleteRenderbuffers(1, &framebuffer->rbo);
		}

		for (uint8_t i = 0; i < MAX_FRAMEBUFFER_COLOR; i++) {
			texture_t* col_texture = &framebuffer->colorTextures[i]; 
			if (col_texture->id == 0) continue;

			Texture_Destroy(col_texture);
		}

		if (framebuffer->depthTexture.id > 0) {
			Texture_Destroy(&framebuffer->depthTexture);
		}

		return false;
	}

	uint8_t colorCount = 0;
	GLenum colorEnums[MAX_FRAMEBUFFER_COLOR] = { 0 };
	for (uint8_t i = 0; i < attachment_count; i++) {
		colorEnums[i] = GL_COLOR_ATTACHMENT0 + i;
		colorCount = i + 1;
	}

	if (colorCount > 0) {
		glNamedFramebufferDrawBuffers(framebuffer->id, colorCount, colorEnums);
	}
	else {
		glNamedFramebufferDrawBuffer(framebuffer->id, 0);
		glNamedFramebufferReadBuffer(framebuffer->id, 0);
	}

	framebuffer->id = framebuffer->id;

	return true;
}

void Framebuffer_Destroy(framebuffer_t* framebuffer)
{
	glDeleteFramebuffers(1, &framebuffer->id);
	
	for (uint8_t i = 0; i < MAX_FRAMEBUFFER_COLOR; i++) {
		texture_t* texture = &framebuffer->colorTextures[i];
		if (texture->id == 0) continue;

		Texture_Destroy(texture);
	}

	if (framebuffer->rbo != 0) {
		glDeleteRenderbuffers(1, &framebuffer->rbo);
	} else {
		Texture_Destroy(&framebuffer->depthTexture); // If we don't have RBO then we must have Depth Texture
	}
}

void Framebuffer_CopyTo(framebuffer_t* framebuffer, framebuffer_t* to, bool include_depth)
{	
	if (!to) {
		ivec2_t windowSize = R_GetWindowSize();
		glBlitNamedFramebuffer(framebuffer->id, 0, 0, 0, framebuffer->width, framebuffer->height, 0, 0, windowSize.x, windowSize.y, include_depth ? GL_DEPTH_BUFFER_BIT : GL_COLOR_BUFFER_BIT, GL_NEAREST);
	} else {
		glBlitNamedFramebuffer(framebuffer->id, to->id, 0, 0, framebuffer->width, framebuffer->height, 0, 0, to->width, to->height, include_depth ? GL_DEPTH_BUFFER_BIT : GL_COLOR_BUFFER_BIT, GL_NEAREST);
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

	ivec2_t windowSize = R_GetWindowSize();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowSize.x, windowSize.y);
}
