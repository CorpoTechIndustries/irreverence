#include <engine/renderer/texturearray.h>

#include <math.h>

#include <GL/glew.h>

bool TextureArray_InitFromMemory(texture_array_t* tex_array, const uint8_t** data, uint8_t texture_count, uint32_t width, uint32_t height, uint8_t channel_count, bool linearize, bool gen_mipmaps)
{
	tex_array->width = width;
	tex_array->height = height;
	tex_array->channelCount = channel_count;

	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &tex_array->id);

	glTextureParameteri(tex_array->id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(tex_array->id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
		glTextureParameterf(tex_array->id, GL_TEXTURE_MAX_ANISOTROPY, fmax(fmin(16.0f, maxAllowAniso), 0.0f));

		glTextureStorage3D(tex_array->id, (GLsizei)(1 + floorf(log2f((width, height)))), glFormat, width, height, texture_count);

		for (uint8_t i = 0; i < texture_count; i++) {
			glTextureSubImage3D(tex_array->id, 0, 0, 0, i, width, height, 1, glFormatAlt, GL_UNSIGNED_BYTE, data[i]);
		}

		glGenerateTextureMipmap(tex_array->id);

	} else {
		GLenum minFilEnum = linearize ? GL_LINEAR : GL_NEAREST;
		glTextureParameteri(tex_array->id, GL_TEXTURE_MIN_FILTER, minFilEnum);
		glTextureParameteri(tex_array->id, GL_TEXTURE_MAG_FILTER, minFilEnum);

		glTextureStorage3D(tex_array->id, 1, glFormat, width, height, texture_count);

		if (data) {
			for (uint8_t i = 0; i < texture_count; i++) {
				glTextureSubImage3D(tex_array->id, 0, 0, 0, i, width, height, 1, glFormatAlt, GL_UNSIGNED_BYTE, data[i]);
			}
		}
	}

	return true;
}

void TextureArray_Destroy(texture_array_t* tex_array)
{
	glDeleteTextures(1, &tex_array->id);
}

static uint32_t s_iBoundTexArrays[MAX_TEXTURE_ARRAY_BINDS] = { 0 }; // Should be linked to Textures but who cares
void TextureArray_Bind(texture_array_t* tex_array, uint8_t location)
{
	if (s_iBoundTexArrays[location] == tex_array->id) return;
	s_iBoundTexArrays[location] = tex_array->id;

	glBindTextureUnit(location, tex_array->id);
}
