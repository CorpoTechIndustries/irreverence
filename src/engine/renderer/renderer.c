#include <engine/renderer/renderer.h>

#include <engine/renderer/lighting.h>
#include <engine/renderer/texture.h>
#include <engine/renderer/uniform.h>
#include <engine/log.h>
#include <engine/input.h>
#include <engine/engine.h>

#include <math/mat4.h>

#include <platform/memory.h>

#include <util/array.h>
#include <util/sort.h>

#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdalign.h>

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
		LOG_INFO("GL Message %i, Source %s, Type %s:: %s", id, sourceStr, typeStr, message);
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		LOG_WARNING("GL Message %i, Source %s, Type %s:: %s", id, sourceStr, typeStr, message);
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		LOG_ERROR("GL Message %i, Source %s, Type %s:: %s", id, sourceStr, typeStr, message);
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

static uniform_t s_AnimationUniform;

static texture_t s_MissingTexture;
static texture_t s_WhiteTexture;
static texture_t s_BlackTexture;

static material_t s_WhiteMaterial;

static mesh_t s_CubeMesh;

bool R_Init()
{
	// Init OpenGL

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		LOG_FATAL("Failed to init GLEW: %s", glewGetErrorString(res));
		return false;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glMessageCallback, NULL);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Init Objects

	g_View = (view_t) {
		.position = NEW_VEC3(0.0f, 0.0f, 0.0f),
		.direction = NEW_VEC3(0.0f, 0.0f, -1.0f),
		.up = NEW_VEC3(0.0f, 1.0f, 0.0f),
		.fov = 70.0f,
		.nearZ = 0.2f,
		.farZ = 1000.0f
	};

	Uniform_Init(&s_GlobalUniform, UNIFORM_LOCATION_GLOBAL, NULL, sizeof(s_GlobalData));

	Uniform_Init(&s_AnimationUniform, UNIFORM_LOCATION_ANIMATIONS, NULL, sizeof(dualquat_t) * 255);

	Light_Init();

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

	texture_t* whiteMaterialTex[] = { &s_WhiteTexture };
	Material_Init(&s_WhiteMaterial, whiteMaterialTex, 1, 0);

	const mesh_vertexmodel_t cubeVertices[] = {
		{ -1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f,			0.0f, 1.0f, 0.0f,		0.0f, 0.0f },
		{ 1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f,			0.0f, 0.0f, 1.0f,		1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f },
		{ 1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f },
		{ -1.0f, -1.0f, -1.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f },
		{ -1.0f, -1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f },
		{ 1.0f, -1.0f, -1.0f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f },
		{ -1.0f, -1.0f, 1.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f },
		{ -1.0f, -1.0f, -1.0f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f },
		{ 1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f },
		{ 1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f },
		{ -1.0f, 1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f },
		{ 1.0f, -1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f },
		{ -1.0f, -1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f },
		{ -1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f },
		{ -1.0f, 1.0f, -1.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f },
		{ 1.0f, -1.0f, 1.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f,			1.0f, 0.0f, 0.0f,		1.0f, 0.0f },
		{ 1.0f, 1.0f, -1.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f }
	};
	const uint32_t cubeIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 0, 18, 1, 3, 19, 4, 6, 20, 7, 9, 21, 10, 12, 22, 13, 15, 23, 16 };
	Mesh_InitModel(&s_CubeMesh, cubeVertices, 24, cubeIndices, 36);

	return true;
}

void R_Destroy()
{
	Uniform_Destroy(&s_GlobalUniform);
	Uniform_Destroy(&s_AnimationUniform);

	Texture_Destroy(&s_MissingTexture);
	Texture_Destroy(&s_WhiteTexture);
	Texture_Destroy(&s_BlackTexture);

	Material_Destroy(&s_WhiteMaterial);

	Mesh_Destroy(&s_CubeMesh);

	Light_Destroy();
}

void R_WindowUpdate(int width, int height)
{
	s_GlobalData.width = width;
	s_GlobalData.height = height;

	glViewport(0, 0, width, height);
}

void R_DebugMoveUpdate()
{
	vec3_t side = VEC3_ZERO;
	Vec3_Cross(g_View.direction, g_View.up, &side);
	Vec3_Normalize(side, &side);

	float ft = Engine_FrameTime();

	if (IN_IsKeyDown(GLFW_KEY_UP)) {
		Vec3_Rotate(120.0f * ft, side, &g_View.direction);
	}

	if (IN_IsKeyDown(GLFW_KEY_DOWN)) {
		Vec3_Rotate(-120.0f * ft, side, &g_View.direction);
	}

	if (IN_IsKeyDown(GLFW_KEY_LEFT)) {
		Vec3_Rotate(120.0f * ft, NEW_VEC3(0.0f, 1.0f, 0.0f), &g_View.direction);
	}

	if (IN_IsKeyDown(GLFW_KEY_RIGHT)) {
		Vec3_Rotate(-120.0f * ft, NEW_VEC3(0.0f, 1.0f, 0.0f), &g_View.direction);
	}

	Vec3_Cross(g_View.direction, g_View.up, &side);
	Vec3_Normalize(side, &side);

	if (IN_IsKeyDown(GLFW_KEY_W)) {
		Vec3_AddTo(g_View.position, Vec3_Mul(g_View.direction, NEW_VEC3S(5.0f * ft)), &g_View.position);
	}

	if (IN_IsKeyDown(GLFW_KEY_S)) {
		Vec3_SubTo(g_View.position, Vec3_Mul(g_View.direction, NEW_VEC3S(5.0f * ft)), &g_View.position);
	}

	if (IN_IsKeyDown(GLFW_KEY_A)) {
		Vec3_SubTo(g_View.position, Vec3_Mul(side, NEW_VEC3S(5.0f * ft)), &g_View.position);
	}

	if (IN_IsKeyDown(GLFW_KEY_D)) {
		Vec3_AddTo(g_View.position, Vec3_Mul(side, NEW_VEC3S(5.0f * ft)), &g_View.position);
	}

	if (IN_IsKeyDown(GLFW_KEY_SPACE)) {
		g_View.position.y += 5.0f * ft;
	}

	if (IN_IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
		g_View.position.y -= 5.0f * ft;
	}
}

void R_Present()
{
	s_GlobalData.curtime = Engine_CurTime();
	s_GlobalData.frametime = Engine_FrameTime();

	float aspect = (float)s_GlobalData.width / (float)s_GlobalData.height;
	Mat4_LookAt(g_View.position, Vec3_Add(g_View.position, g_View.direction), g_View.up, &s_GlobalData.view);
	Mat4_Perspective(g_View.fov, aspect, g_View.nearZ, g_View.farZ, &s_GlobalData.projection);

	Uniform_Update(&s_GlobalUniform, &s_GlobalData, sizeof(s_GlobalData), 0);

	Light_Update();
}

void R_UpdateAnimationBuffer(animator_t* animator)
{
	Uniform_Update(&s_AnimationUniform, animator->finalMatrices, sizeof(dualquat_t) * animator->animation->bones.count, 0);
}

ivec2_t R_GetWindowSize()
{
	return NEW_IVEC2(s_GlobalData.width, s_GlobalData.height);
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

material_t* R_GetWhiteMaterial()
{
	return &s_WhiteMaterial;
}

mesh_t* R_GetCubeMesh()
{
	return &s_CubeMesh;
}

view_t g_View;
