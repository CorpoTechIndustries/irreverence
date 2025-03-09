#include <engine/renderer/shader.h>

#include <GL/glew.h>

#include <string.h>
#include <stdio.h>
#include <malloc.h>

static bool ReadShaderFile(const char* path, char** out_buffer, size_t* out_size)
{
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		return false;
	}

	fseek(file, 0, SEEK_END);
	int64_t fsize = ftell(file);
	rewind(file);

	*out_buffer = malloc(fsize + 1);
	fread(*out_buffer, fsize, 1, file);
	(*out_buffer)[fsize] = '\0';
	fclose(file);

	*out_size = fsize;

	return true;
} 

bool Shader_Init(shader_t* shader, const char* name, const char* vertex_path, const char* pixel_path)
{
	shader->programId = 0;

	if (!vertex_path || !pixel_path) return false;

	char failureLog[1024];
	int bSuccess = true;

	unsigned int v_id = 0;
	unsigned int p_id = 0;

	strncpy(shader->name, name, MAX_SHADER_NAME);
	char* vertexCode = NULL;
	size_t vertexCodeSize = 0;
	if (!ReadShaderFile(vertex_path, &vertexCode, &vertexCodeSize)) {
		printf("Failed to load Vertex code from path \"%s\"\n", vertex_path);
		return false;
	}

	v_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v_id, 1, (const GLchar* const*)&vertexCode, NULL);
	glCompileShader(v_id);
	glGetShaderiv(v_id, GL_COMPILE_STATUS, &bSuccess);

	free(vertexCode);

	bool bFailedShaderCompilation = false;
	if (!bSuccess) {
		glGetShaderInfoLog(v_id, 1024, NULL, failureLog);
		printf("Vertex shader compilation failed:\n %s\n", failureLog);
		bFailedShaderCompilation = true;
	}

	char* pixelCode = NULL;
	size_t pixelCodeSize = 0;
	if (!ReadShaderFile(pixel_path, &pixelCode, &pixelCodeSize)) {
		printf("Failed to load Pixel code from path \"%s\"\n", pixel_path);
		return false;
	}

	p_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(p_id, 1, (const GLchar* const*)&pixelCode, NULL);
	glCompileShader(p_id);
	glGetShaderiv(p_id, GL_COMPILE_STATUS, &bSuccess);

	free(pixelCode);

	if (!bSuccess) {
		glGetShaderInfoLog(p_id, 1024, NULL, failureLog);
		printf("Pixel shader compilation failed:\n %s\n", failureLog);
		bFailedShaderCompilation = true;
	}

	if (bFailedShaderCompilation) {
		glDeleteShader(v_id);
		glDeleteShader(p_id);
		return false;
	}

	unsigned int id = glCreateProgram();
	glAttachShader(id, v_id);
	glAttachShader(id, p_id);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &bSuccess);

	if (!bSuccess) {
		// Linking failed.
		glGetProgramInfoLog(id, 1024, NULL, failureLog);
		printf("Shader Program Linking failed:\n %s\n", failureLog);
		glDeleteShader(v_id);
		glDeleteShader(p_id);

		glDeleteProgram(id);
		return 0;
	}

	glDeleteShader(v_id);
	glDeleteShader(p_id);

	shader->programId = id;

	return true;
}

void Shader_Destroy(shader_t* shader)
{
	glDeleteProgram(shader->programId);
	shader->programId = 0;
}

static uint32_t s_iBoundShader = 0;
void Shader_Bind(shader_t* shader)
{
	if (s_iBoundShader == shader->programId) return;
	s_iBoundShader = shader->programId;

	glUseProgram(shader->programId);
}