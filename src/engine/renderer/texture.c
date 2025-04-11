#include <engine/renderer/texture.h>

#include <engine/renderer/framebuffer.h>
#include <engine/log.h>

#include <math.h>

#include <stb_image.h>
#include <GL/glew.h>

bool Texture_Init(texture_t* texture, const char* image_path, bool linearize, bool gen_mipmaps)
{
	if (!image_path) return false;

	stbi_set_flip_vertically_on_load(true);

	int width = 0, height = 0, channels = 0;
	unsigned char* data = stbi_load(image_path, &width, &height, &channels, 0);

	if (!data) {
		LOG_ERROR("Failed to create texture from image \"%s\", reason: %s", image_path, stbi_failure_reason());
		return false;
	}

	Texture_InitFromMemory(texture, data, width, height, channels, linearize, gen_mipmaps);

	stbi_image_free(data);

	return true;
}

bool Texture_InitFromMemory(texture_t* texture, const uint8_t* data, uint32_t width, uint32_t height, uint8_t channel_count, bool linearize, bool gen_mipmaps)
{
	texture->width = width;
	texture->height = height;
	texture->channelCount = channel_count;
	texture->mipCount = 0;

	glCreateTextures(GL_TEXTURE_2D, 1, &texture->id);

	glTextureParameteri(texture->id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture->id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLenum glFormat = GL_RGBA8;
	GLenum glFormatAlt = GL_RGBA;
	if (channel_count == 4)  {
		glFormat = GL_RGBA8;
		glFormatAlt = GL_RGBA;
	} else {
		glFormat = GL_RGB8;
		glFormatAlt = GL_RGB;
	}

	if (gen_mipmaps && data) {
		GLenum minFilEnum = linearize ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
		GLenum minFilEnumMag = linearize ? GL_LINEAR : GL_NEAREST;

		float maxAllowAniso = 0.0f; glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAllowAniso);
		glTextureParameterf(texture->id, GL_TEXTURE_MAX_ANISOTROPY, fmax(fmin(16.0f, maxAllowAniso), 0.0f));

		texture->mipCount = (uint32_t)(1 + floorf(log2f(MATH_MAX(width, height))));
		glTextureStorage2D(texture->id, texture->mipCount, glFormat, width, height);
		glTextureSubImage2D(texture->id, 0, 0, 0, width, height, glFormatAlt, GL_UNSIGNED_BYTE, data);

		glGenerateTextureMipmap(texture->id);
	} else {
		GLenum minFilEnum = linearize ? GL_LINEAR : GL_NEAREST;
		glTextureParameteri(texture->id, GL_TEXTURE_MIN_FILTER, minFilEnum);
		glTextureParameteri(texture->id, GL_TEXTURE_MAG_FILTER, minFilEnum);

		glTextureStorage2D(texture->id, 1, glFormat, width, height);

		if (data) {
			glTextureSubImage2D(texture->id, 0, 0, 0, width, height, glFormatAlt, GL_UNSIGNED_BYTE, data);
		}
	}

	return true;
}

bool Texture_InitColorAttachment(
	texture_t* texture,
	framebuffer_t* framebuffer,
	uint8_t location,
	uint32_t width,
	uint32_t height,
	uint8_t samples,
	gapi_enum_t format,
	gapi_enum_t type,
	uint8_t max_mips)
{
	texture->width = width;
	texture->height = height;
	texture->channelCount = 0;
	texture->mipCount = 0;

	if (samples == 0 || max_mips > 1) {
		glCreateTextures(GL_TEXTURE_2D, 1, &texture->id);
		glTextureParameteri(texture->id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(texture->id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(texture->id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(texture->id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		texture->mipCount = (max_mips > 1) ? (uint8_t)(1 + floorf(log2f(MATH_MAX(width, height)))) : 1;
		texture->mipCount = MATH_MIN(texture->mipCount, max_mips);

		glTextureStorage2D(texture->id, texture->mipCount, format, width, height);
		
		glNamedFramebufferTexture(framebuffer->id, GL_COLOR_ATTACHMENT0 + location, texture->id, 0);
	} else {
		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &texture->id);
		glTextureStorage2DMultisample(texture->id, samples, type, width, height, GL_TRUE);

		glNamedFramebufferTexture(framebuffer->id, GL_COLOR_ATTACHMENT0 + location, texture->id, 0);
	}

	return true;
}

bool Texture_InitDepthAttachment(
	texture_t* texture,
	framebuffer_t* framebuffer,
	uint32_t width,
	uint32_t height,
	uint8_t samples,
	gapi_enum_t format,
	gapi_enum_t type)
{
	texture->width = width;
	texture->height = height;
	texture->channelCount = 0;
	texture->mipCount = 0;

	if (samples == 0) {
		glCreateTextures(GL_TEXTURE_2D, 1, &texture->id);

		glTextureParameteri(texture->id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(texture->id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(texture->id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(texture->id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureStorage2D(texture->id, 1, format, width, height);
	} else {
		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &texture->id);
		glTextureStorage2DMultisample(texture->id, (GLsizei)samples, type, width, height, GL_TRUE);
	}

	glNamedFramebufferTexture(framebuffer->id, GL_DEPTH_ATTACHMENT, texture->id, 0);

	return true;
}

void Texture_Destroy(texture_t* texture)
{
	glDeleteTextures(1, &texture->id);
	texture->id = 0;
}

static uint32_t s_iBoundTexArrays[MAX_TEXTURE_BINDS] = { 0 };
void Texture_BindRaster(texture_t* texture, uint8_t location)
{
	if (s_iBoundTexArrays[location] == texture->id) return;
	s_iBoundTexArrays[location] = texture->id;

	glBindTextureUnit(location, texture->id);
}

void Texture_BindCompute(texture_t* texture, uint8_t location, uint8_t mip, gapi_enum_t access, gapi_enum_t format)
{
	glBindImageTexture(location, texture->id, mip, GL_FALSE, 0, access, format);
}
