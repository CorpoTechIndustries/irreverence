#include <engine/renderer/renderer.h>

#include <engine/renderer/uniform.h>
#include <engine/renderer/texture.h>
#include <engine/log.h>

#include <math/mat4.h>

#include <platform/memory.h>

#include <string.h>
#include <math.h>

#include <GL/glew.h>

static void glMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	if (type == GL_DEBUG_TYPE_PERFORMANCE) return;

	char sourceStr[16];
	memset(sourceStr, '\0', sizeof(sourceStr));
	switch (source)
	{
	case GL_DEBUG_SOURCE_API: strcpy(sourceStr, "API");
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: strcpy(sourceStr, "WINDOW SYSTEM");
	case GL_DEBUG_SOURCE_SHADER_COMPILER: strcpy(sourceStr, "SHADER COMPILER");
	case GL_DEBUG_SOURCE_THIRD_PARTY: strcpy(sourceStr, "THIRDPARTY");
	case GL_DEBUG_SOURCE_APPLICATION: strcpy(sourceStr, "APPLICATION");
	case GL_DEBUG_SOURCE_OTHER: strcpy(sourceStr, "OTHER");
	};

	char typeStr[20];
	memset(typeStr, '\0', sizeof(typeStr));

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR: strcpy(typeStr, "ERROR");
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: strcpy(typeStr, "DEPRECATED BEHAVIOR");
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: strcpy(typeStr, "UNDEFINED BEHAVIOR");
	case GL_DEBUG_TYPE_PORTABILITY: strcpy(typeStr, "PORTABILITY");
	case GL_DEBUG_TYPE_PERFORMANCE: strcpy(typeStr, "PERFORMANCE");
	case GL_DEBUG_TYPE_MARKER: strcpy(typeStr, "MARKER");
	case GL_DEBUG_TYPE_OTHER: strcpy(typeStr, "OTHER");
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		LOG_INFO("GL Message %i, Source %s, Type %s:: %s\n", id, sourceStr, typeStr, message);
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		LOG_WARNING("GL Message %i, Source %s, Type %s:: %s\n", id, sourceStr, typeStr, message);
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		LOG_ERROR("GL Message %i, Source %s, Type %s:: %s\n", id, sourceStr, typeStr, message);
		break;
	}
}

static uniform_t s_GlobalUniform;
static struct {
	uint32_t width;
	uint32_t height;
	float curtime;
	float frametime;
	mat4_t view;
	mat4_t projection;
} s_GlobalData;

static texture_t s_MissingTexture;
static texture_t s_WhiteTexture;
static texture_t s_BlackTexture;

bool R_Init()
{
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		LOG_FATAL("Failed to init GLEW: %s\n", glewGetErrorString(res));
		return false;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glMessageCallback, NULL);

	Uniform_Init(&s_GlobalUniform, 0, NULL, sizeof(s_GlobalData));

	struct UCRGB {
		uint8_t r, g, b;
	};

	const struct UCRGB missingColor1 = { 0, 0, 0 };
	const struct UCRGB missingColor2 = { 251, 62, 249 };
	const struct UCRGB whiteColor = { 255, 255, 255 };

	const struct UCRGB missingPixels[] = {
		missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2,
		missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1,
		missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2,
		missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1,
		missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2,
		missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1,
		missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2,
		missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1, missingColor2, missingColor1
	};

	Texture_InitFromMemory(&s_MissingTexture, (const uint8_t*)missingPixels, 8, 8, 3, false, false);
	Texture_InitFromMemory(&s_BlackTexture, (const uint8_t*)&missingColor1, 1, 1, 3, false, false);
	Texture_InitFromMemory(&s_WhiteTexture, (const uint8_t*)&whiteColor, 1, 1, 3, false, false);

	return true;
}

void R_Destroy()
{
	Uniform_Destroy(&s_GlobalUniform);

	Texture_Destroy(&s_MissingTexture);
	Texture_Destroy(&s_WhiteTexture);
	Texture_Destroy(&s_BlackTexture);
}

void R_Present()
{
	s_GlobalData.width = 1280;
	s_GlobalData.height = 720;
	s_GlobalData.curtime = 0.0f;
	s_GlobalData.frametime = 0.0f;

	s_GlobalData.view = MAT4_IDENTITY;
	s_GlobalData.projection = MAT4_IDENTITY;

	Uniform_Update(&s_GlobalUniform, &s_GlobalData, sizeof(s_GlobalData), 0);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

texture_t* R_GetMissingTexture()
{
	return &s_MissingTexture;
}

texture_t* R_GetWhiteTexture()
{
	return &s_WhiteTexture;
}

texture_t* R_GetBlackTexture()
{
	return &s_BlackTexture;
}