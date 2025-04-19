#include <engine/renderer/shader.h>
#include <engine/log.h>

#include <platform/path.h>
#include <platform/memory.h>

#include <GL/glew.h>

#include <string.h>

static bool ReadShaderFile(const char* path, char** out_buffer, size_t* out_size)
{
	char* text = Sys_PathReadFile(path, out_size);

	if (!text) {
		return false;
	}

	*out_buffer = text;

	return true;
}

bool Shader_InitRaster(shader_t* shader, const char* name, const char* vertex_path, const char* pixel_path)
{
	if (!vertex_path || !pixel_path) return false;

	strncpy(shader->name, name, MAX_SHADER_NAME_LENGTH);
	
	char* vertexCode = NULL;
	size_t vertexCodeSize = 0;
	if (!ReadShaderFile(vertex_path, &vertexCode, &vertexCodeSize)) {
		LOG_ERROR("Failed to load Vertex code from path \"%s\"", vertex_path);
		return false;
	}
	
	char* pixelCode = NULL;
	size_t pixelCodeSize = 0;
	if (!ReadShaderFile(pixel_path, &pixelCode, &pixelCodeSize)) {
		LOG_ERROR("Failed to load Pixel code from path \"%s\"", pixel_path);
		Sys_Free(vertexCode);
		return false;
	}
	
	bool res = Shader_InitRasterFromMemory(shader, vertexCode, pixelCode);

	Sys_Free(vertexCode);
	Sys_Free(pixelCode);

	return res;
}

bool Shader_InitRasterFromMemory(shader_t* shader, const char* vertex_code, const char* pixel_code)
{
	shader->id = 0;

	if (!vertex_code || !pixel_code) return false;

	char failureLog[1024];
	int bSuccess = true;

	unsigned int v_id = 0;
	unsigned int p_id = 0;

	v_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v_id, 1, (const GLchar* const*)&vertex_code, NULL);
	glCompileShader(v_id);
	glGetShaderiv(v_id, GL_COMPILE_STATUS, &bSuccess);
	
	bool bFailedShaderCompilation = false;
	if (!bSuccess) {
		glGetShaderInfoLog(v_id, 1024, NULL, failureLog);
		LOG_ERROR("Vertex shader compilation failed:\n %s", failureLog);
		bFailedShaderCompilation = true;
	}

	p_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(p_id, 1, (const GLchar* const*)&pixel_code, NULL);
	glCompileShader(p_id);
	glGetShaderiv(p_id, GL_COMPILE_STATUS, &bSuccess);

	if (!bSuccess) {
		glGetShaderInfoLog(p_id, 1024, NULL, failureLog);
		LOG_ERROR("Pixel shader compilation failed:\n %s", failureLog);
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
		LOG_ERROR("Shader Program Linking failed:\n %s", failureLog);
		glDeleteShader(v_id);
		glDeleteShader(p_id);

		glDeleteProgram(id);
		return false;
	}

	glDeleteShader(v_id);
	glDeleteShader(p_id);

	shader->id = id;

	return true;
}

bool Shader_InitCompute(shader_t* shader, const char* name, const char* path)
{
	if (!path) return false;

	strncpy(shader->name, name, MAX_SHADER_NAME_LENGTH);

	char* computeCode = NULL;
	size_t computeCodeSize = 0;
	if (!ReadShaderFile(path, &computeCode, &computeCodeSize)) {
		LOG_ERROR("Failed to load Compute code from path \"%s\"", path);
		return false;
	}

	bool res = Shader_InitComputeFromMemory(shader, computeCode);

	Sys_Free(computeCode);
	
	return true;
}

bool Shader_InitComputeFromMemory(shader_t* shader, const char* code)
{
	shader->id = 0;

	if (!code) return false;

	char failureLog[1024];
	int bSuccess = true;

	unsigned int c_id = 0;

	c_id = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(c_id, 1, (const GLchar* const*)&code, NULL);
	glCompileShader(c_id);
	glGetShaderiv(c_id, GL_COMPILE_STATUS, &bSuccess);
	
	if (!bSuccess) {
		glGetShaderInfoLog(c_id, 1024, NULL, failureLog);
		LOG_ERROR("Compute shader compilation failed:\n %s", failureLog);
		glDeleteShader(c_id);
		return false;
	}

	unsigned int id = glCreateProgram();
	glAttachShader(id, c_id);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &bSuccess);

	if (!bSuccess) {
		// Linking failed.
		glGetProgramInfoLog(id, 1024, NULL, failureLog);
		LOG_ERROR("Shader Program Linking failed:\n %s", failureLog);
		glDeleteShader(c_id);

		glDeleteProgram(id);
		return false;
	}

	glDeleteShader(c_id);

	shader->id = id;

	return true;
}

void Shader_Destroy(shader_t* shader)
{
	glDeleteProgram(shader->id);
	shader->id = 0;
}

static uint32_t s_iBoundShader = 0;
void Shader_Bind(shader_t* shader)
{
	if (s_iBoundShader == shader->id) return;
	s_iBoundShader = shader->id;

	glUseProgram(shader->id);
}

void Shader_Dispatch(shader_t* shader, ivec3_t groups, gapi_enum_t memory_barrier)
{
	glDispatchCompute(groups.x, groups.y, groups.z);
	
	if (memory_barrier != 0) {
		glMemoryBarrier(memory_barrier);
	}
}

uint8_t Shader_GetLocation(shader_t* shader, const char* name)
{
	// TODO: use Map to find the value

	return glGetUniformLocation(shader->id, name);
}

void Shader_SetInt(shader_t* shader, uint8_t location, int32_t v)
{
 	glProgramUniform1i(shader->id, location, v);
}

void Shader_SetUInt(shader_t* shader, uint8_t location, uint32_t v)
{
	glProgramUniform1ui(shader->id, location, v);
}

void Shader_SetFloat(shader_t* shader, uint8_t location, float v)
{
	glProgramUniform1f(shader->id, location, v);
}

void Shader_SetVec2(shader_t* shader, uint8_t location, vec2_t v)
{
	glProgramUniform2f(shader->id, location, v.x, v.y);
}

void Shader_SetVec3(shader_t* shader, uint8_t location, vec3_t v)
{
	glProgramUniform3f(shader->id, location, v.x, v.y, v.z);
}
void Shader_SetVec4(shader_t* shader, uint8_t location, vec4_t v)
{
	glProgramUniform4f(shader->id, location, v.x, v.y, v.z, v.w);
}

void Shader_SetIVec2(shader_t* shader, uint8_t location, ivec2_t v)
{
	glProgramUniform2i(shader->id, location, v.x, v.y);
}

void Shader_SetIVec3(shader_t* shader, uint8_t location, ivec3_t v)
{
	glProgramUniform3i(shader->id, location, v.x, v.y, v.z);
}

void Shader_SetIVec4(shader_t* shader, uint8_t location, ivec4_t v)
{
	glProgramUniform4i(shader->id, location, v.x, v.y, v.z, v.w);
}
