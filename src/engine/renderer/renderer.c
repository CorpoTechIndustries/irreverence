#include <engine/renderer/renderer.h>

#include <engine/renderer/lighting.h>
#include <engine/renderer/texture.h>
#include <engine/renderer/uniform.h>
#include <engine/renderer/shader.h>
#include <engine/renderer/framebuffer.h>
#include <engine/renderer/storagebuffer.h>
#include <engine/renderer/mesh.h>
#include <engine/renderer/animation.h>
#include <engine/renderer/material.h>
#include <engine/renderer/batch.h>
#include <engine/renderer/font.h>
#include <engine/log.h>
#include <engine/input.h>
#include <engine/engine.h>

#include <math/mat4.h>

#include <platform/memory.h>

#include <util/array.h>
#include <util/sort.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string.h>
#include <math.h>
#include <stdalign.h>

#include <engine/client.h>

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

static uniform_t s_UniformCommon;
static struct common_data {
	uint32_t width;
	uint32_t height;
	float curtime;
	float frametime;
	mat4_t view;
	mat4_t projection;
	float near;
	float far;
} s_CommonData;

static uniform_t s_UniformAnimation;

static framebuffer_t s_FBMain;
static framebuffer_t s_FBBloom;

static shader_t s_ShaderOpaqueMdl;
static shader_t s_ShaderTranspMdl;
static shader_t s_ShaderOverlayTransp;
static shader_t s_ShaderOverlayScreen;
static shader_t s_ShaderOverlayBloomDS;
static shader_t s_ShaderOverlayBloomUS;
static shader_t s_ShaderOverlayText;
static shader_t s_ShaderOverlayPrimitive;

static texture_t s_TextureMissing;
static texture_t s_TextureWhite;
static texture_t s_TextureBlack;

static material_t s_MaterialWhite;

static mesh_t s_MeshCube;
static mesh_t s_MeshRect;
static mesh_t s_MeshText;

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
		.farZ = 500.0f
	};

	Uniform_Init(&s_UniformCommon, UNIFORM_LOCATION_COMMON, NULL, sizeof(s_CommonData));

	dualquat_t defAnimQuats[255];
	
	for (uint32_t i = 0; i < 255; i++) {
		defAnimQuats[i] = DUALQUAT_IDENTITY;
	}

	Uniform_Init(&s_UniformAnimation, UNIFORM_LOCATION_ANIMATIONS, defAnimQuats, sizeof(dualquat_t) * 255);

	Light_Init();

	framebuffer_attachment_t mainFBAttachments[] = {
		{ .format = GL_RGB16F, .type = GL_HALF_FLOAT, .maxMips = 1 }, 	// Main Color
		{ .format = GL_RGBA16F, .type = GL_HALF_FLOAT, .maxMips = 1 }, 	// Accum Color
		{ .format = GL_R16F, .type = GL_FLOAT, .maxMips = 1 } 		// Reveal
	};
	framebuffer_attachment_t mainFBDepth = { .format = GL_DEPTH_COMPONENT24, .type = GL_FLOAT };

	Framebuffer_Init(&s_FBMain, 1280, 720, 0, mainFBAttachments, 3, &mainFBDepth);

	framebuffer_attachment_t bloomFBAttachments[] = {
		{ .format = GL_RGB16F, .type = GL_HALF_FLOAT, .maxMips = 8 } 	// Downscale
	};
	Framebuffer_Init(&s_FBBloom, 1280, 720, 0, bloomFBAttachments, 1, NULL);

	Shader_InitRaster(&s_ShaderOpaqueMdl, "ENGINE_OPAQUE_MODEL", "assets/shaders/OpaqueModel.vert", "assets/shaders/OpaqueModel.frag");
	Shader_InitRaster(&s_ShaderTranspMdl, "ENGINE_TRANSP_MODEL", "assets/shaders/OpaqueModel.vert", "assets/shaders/TranspModel.frag");
	Shader_InitRaster(&s_ShaderOverlayTransp, "ENGINE_OVERLAY_TRANSP", "assets/shaders/OverlayScreen.vert", "assets/shaders/OverlayTransp.frag");
	Shader_InitRaster(&s_ShaderOverlayScreen, "ENGINE_OVERLAY_SCREEN", "assets/shaders/OverlayScreen.vert", "assets/shaders/OverlayScreen.frag");
	Shader_InitRaster(&s_ShaderOverlayBloomDS, "ENGINE_OVERLAY_BLOOMDS", "assets/shaders/OverlayScreen.vert", "assets/shaders/OverlayBloomDS.frag");
	Shader_InitRaster(&s_ShaderOverlayBloomUS, "ENGINE_OVERLAY_BLOOMUS", "assets/shaders/OverlayScreen.vert", "assets/shaders/OverlayBloomUS.frag");
	Shader_InitRaster(&s_ShaderOverlayText, "ENGINE_OVERLAY_TEXT", "assets/shaders/OverlayText.vert", "assets/shaders/OverlayText.frag");
	Shader_InitRaster(&s_ShaderOverlayPrimitive, "ENGINE_OVERLAY_PRIMITIVE", "assets/shaders/OverlayPrimitive.vert", "assets/shaders/OverlayPrimitive.frag");

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

	Texture_InitFromMemory(&s_TextureMissing, (const uint8_t*)missingPixels, 8, 8, 3, false, false);
	Texture_InitFromMemory(&s_TextureBlack, (const uint8_t*)&missingColor1, 1, 1, 3, false, false);
	Texture_InitFromMemory(&s_TextureWhite, (const uint8_t*)&whiteColor, 1, 1, 3, false, false);

	texture_t* whiteMaterialTex[] = { &s_TextureWhite };
	Material_InitTextures(&s_MaterialWhite, whiteMaterialTex, 1, 0);

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
	Mesh_InitModel(&s_MeshCube, cubeVertices, 24, cubeIndices, 36);
	
	const float rectVertices[] = {
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	const uint32_t rectIndices[] = { 2, 1, 0, 2, 3, 1 };
	const mesh_attribute_t rectVertAttribs[] = {
		{ .type = GL_FLOAT, .count = 2 }, 	// Pos
		{ .type = GL_FLOAT, .count = 2 } 	// UV
	};
	const mesh_attribute_t rectInstAttribs[] = {
		{ .type = GL_FLOAT, .count = 4 }, 	// Color
		{ .type = GL_FLOAT, .count = 2 }, 	// Pos
		{ .type = GL_FLOAT, .count = 2 } 	// Size
	};
	Mesh_InitCustom(&s_MeshRect, rectVertices, sizeof(rectVertices), rectIndices, 6, rectVertAttribs, 2, rectInstAttribs, 3);
	
	const mesh_attribute_t textVertAttribs[] = { 
		{ .type = GL_FLOAT, .count = 2 }, 	// Pos
		{ .type = GL_FLOAT, .count = 2 } 	// UV
	};
	const mesh_attribute_t textInstAttribs[] = { 
		{ .type = GL_FLOAT, .count = 4 }, 	// Color
		{ .type = GL_FLOAT, .count = 2 }, 	// Pos
		{ .type = GL_FLOAT, .count = 2 }, 	// Size
		{ .type = GL_FLOAT, .count = 4 } 	// UVs
	};
	Mesh_InitCustom(&s_MeshText, rectVertices, sizeof(rectVertices), rectIndices, 6, textVertAttribs, 2, textInstAttribs, 4);

	return true;
}

void R_Destroy()
{
	Uniform_Destroy(&s_UniformCommon);
	Uniform_Destroy(&s_UniformAnimation);
	Light_Destroy();

	Framebuffer_Destroy(&s_FBMain);
	Framebuffer_Destroy(&s_FBBloom);

	Shader_Destroy(&s_ShaderOpaqueMdl);
	Shader_Destroy(&s_ShaderTranspMdl);
	Shader_Destroy(&s_ShaderOverlayTransp);
	Shader_Destroy(&s_ShaderOverlayScreen);
	Shader_Destroy(&s_ShaderOverlayBloomDS);
	Shader_Destroy(&s_ShaderOverlayBloomUS);
	Shader_Destroy(&s_ShaderOverlayText);
	Shader_Destroy(&s_ShaderOverlayPrimitive);

	Texture_Destroy(&s_TextureMissing);
	Texture_Destroy(&s_TextureWhite);
	Texture_Destroy(&s_TextureBlack);

	Material_Destroy(&s_MaterialWhite);

	Mesh_Destroy(&s_MeshCube);
	Mesh_Destroy(&s_MeshRect);
	Mesh_Destroy(&s_MeshText);
}

void R_WindowUpdate(int width, int height)
{
	s_CommonData.width = width;
	s_CommonData.height = height;

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

static void RenderDepth() { }

static void RenderShadows() { }

static void RenderOpaque()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	Framebuffer_Bind(&s_FBMain);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader_Bind(&s_ShaderOpaqueMdl);
	Texture_BindRaster(R_GetWhiteTexture(), 0);

	mesh_instancemodel_t instdata1 = {
		.r = 10.0f,
		.g = 2.0f,
		.b = 2.0f,
		.a = 1.0f,
		.model = MAT4_IDENTITY
	};
	Mat4_Translate(&instdata1.model, NEW_VEC3(0.0f, 0.0f, -8.0f));
	Mat4_Scale(&instdata1.model, NEW_VEC3(1.0f, 1.0f, 1.0f));

	mesh_instancemodel_t instdata2 = {
		.r = 0.8f,
		.g = 0.8f,
		.b = 0.8f,
		.a = 1.0f,
		.model = MAT4_IDENTITY
	};
	Mat4_Translate(&instdata2.model, NEW_VEC3(0.0f, -2.0f, 0.0f));
	Mat4_Scale(&instdata2.model, NEW_VEC3(10.0f, 1.0f, 10.0f));

	Mesh_Draw(R_GetCubeMesh(), &instdata1);
	Mesh_Draw(R_GetCubeMesh(), &instdata2);
}

static void RenderTransparent()
{
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunciARB(1, GL_ONE, GL_ONE);
	glBlendFunciARB(2, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	glBlendEquation(GL_FUNC_ADD);

	Framebuffer_Bind(&s_FBMain);
	Framebuffer_ClearColor(&s_FBMain, 1, NEW_VEC4S(0.0f));
	Framebuffer_ClearColor(&s_FBMain, 2, NEW_VEC4S(1.0f));

	Shader_Bind(&s_ShaderTranspMdl);
	Texture_BindRaster(R_GetWhiteTexture(), 0);

	mesh_instancemodel_t inst1data = {
		.r = 0.0f,
		.g = 0.0f,
		.b = 1.0f,
		.a = 0.5f,
		.model = MAT4_IDENTITY
	};
	Mat4_Translate(&inst1data.model, NEW_VEC3(0.5f, 0.0f, -4.0f));
	mesh_instancemodel_t inst2data = {
		.r = 0.0f,
		.g = 1.0f,
		.b = 0.0f,
		.a = 0.2f,
		.model = MAT4_IDENTITY
	};
	Mat4_Translate(&inst2data.model, NEW_VEC3(0.0f, 0.0f, -2.0f));

	Mesh_Draw(R_GetCubeMesh(), &inst1data);
	Mesh_Draw(R_GetCubeMesh(), &inst2data);
}

static void RenderDecals() { }

static void RenderOverlay()
{
	const struct {
		float r, g, b, a;
		vec2_t pos;
		vec2_t size;
	} scrRectData = {
		.r = 1.0f,
		.g = 1.0f,
		.b = 1.0f,
		.a = 1.0f,
		.pos = NEW_VEC2(0.0f, 0.0f),
		.size = NEW_VEC2(s_CommonData.width, s_CommonData.height) };

	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	glDepthMask(GL_FALSE);
	Framebuffer_Bind(&s_FBMain);

	Shader_Bind(&s_ShaderOverlayTransp);
	Texture_BindRaster(&s_FBMain.colorTextures[1], 0);
	Texture_BindRaster(&s_FBMain.colorTextures[2], 1);
	Mesh_Draw(&s_MeshRect, &scrRectData);

	glDisable(GL_BLEND);
	
	Framebuffer_Bind(&s_FBBloom);
	Shader_Bind(&s_ShaderOverlayBloomDS);
	Texture_BindRaster(&s_FBMain.colorTextures[0], 0);
	
	Shader_SetUInt(&s_ShaderOverlayBloomDS, 0, 0);
	Shader_SetVec2(&s_ShaderOverlayBloomDS, 1, NEW_VEC2(1.0f / (float)s_FBBloom.width, 1.0f / (float)s_FBBloom.height));
	
	for (uint8_t i = 0; i < s_FBBloom.colorTextures[0].mipCount; i++) {
		uint32_t width = s_FBBloom.width >> i, height = s_FBBloom.height >> i;
		glViewport(0, 0, width, height);
		
		Framebuffer_RelinkAttachment(&s_FBBloom, 0, i);
		
		Mesh_Draw(&s_MeshRect, &scrRectData);
		
		Shader_SetVec2(&s_ShaderOverlayBloomDS, 1, NEW_VEC2(1.0f / (float)width, 1.0f / (float)height));
		Shader_SetUInt(&s_ShaderOverlayBloomDS, 0, i);
		Texture_BindRaster(&s_FBBloom.colorTextures[0], 0);
	}
	Framebuffer_RelinkAttachment(&s_FBBloom, 0, 0);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	Shader_Bind(&s_ShaderOverlayBloomUS);
	Shader_SetUInt(&s_ShaderOverlayBloomUS, 0, 0);
	Shader_SetFloat(&s_ShaderOverlayBloomUS, 1, 0.003f);

	for (uint8_t i = s_FBBloom.colorTextures[0].mipCount - 1; i > 0; i--) {
		uint8_t iNext = i - 1;
		uint32_t width = s_FBBloom.width >> iNext, height = s_FBBloom.height >> iNext;
		
		Texture_BindRaster(&s_FBBloom.colorTextures[0], 0);
		Shader_SetUInt(&s_ShaderOverlayBloomUS, 0, i);

		glViewport(0, 0, width, height);
		Framebuffer_RelinkAttachment(&s_FBBloom, 0, iNext);

		Mesh_Draw(&s_MeshRect, &scrRectData);
	}
	Framebuffer_RelinkAttachment(&s_FBBloom, 0, 0);

	glDisable(GL_BLEND);

	Framebuffer_UnBind();	

	Shader_Bind(&s_ShaderOverlayScreen);
	Texture_BindRaster(&s_FBMain.colorTextures[0], 0);
	Texture_BindRaster(&s_FBBloom.colorTextures[0], 1);
	Mesh_Draw(&s_MeshRect, &scrRectData);
	
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	Shader_Bind(&s_ShaderOverlayText);
}

void R_UpdateAnimationBuffer(animator_t* animator)
{
	Uniform_Update(&s_UniformAnimation, animator->boneQuats, sizeof(dualquat_t) * animator->boneCount, 0);
}

void R_Present()
{
	s_CommonData.curtime = Engine_CurTime();
	s_CommonData.frametime = Engine_FrameTime();
	s_CommonData.near = g_View.nearZ;
	s_CommonData.far = g_View.farZ;

	Mat4_LookAt(g_View.position, Vec3_Add(g_View.position, g_View.direction), g_View.up, &s_CommonData.view);
	Mat4_Perspective(g_View.fov, (float)s_CommonData.width / (float)s_CommonData.height, g_View.nearZ, g_View.farZ, &s_CommonData.projection);

	Uniform_Update(&s_UniformCommon, &s_CommonData, sizeof(s_CommonData), 0);

	Light_Update();

	if (!g_ClientExports.pRenderPreDepth()) RenderDepth();
	g_ClientExports.pRenderPostDepth();

	if (!g_ClientExports.pRenderPreShadows()) RenderShadows();
	g_ClientExports.pRenderPostShadows();

	if (!g_ClientExports.pRenderPreOpaque()) RenderOpaque();
	g_ClientExports.pRenderPostOpaque();

	CL_Render(&cl);

	if (!g_ClientExports.pRenderPreTransparent()) RenderTransparent();
	g_ClientExports.pRenderPostTransparent();

	if (!g_ClientExports.pRenderPreDecals()) RenderDecals();
	g_ClientExports.pRenderPostDecals();

	Mat4_Ortho(0.0f, (float)s_CommonData.width, 0.0f, (float)s_CommonData.height, 0.0f, 1.0f, &s_CommonData.projection);
	Uniform_Update(&s_UniformCommon, &s_CommonData.projection, sizeof(mat4_t), offsetof(struct common_data, projection));

	if (!g_ClientExports.pRenderPreOverlay()) RenderOverlay();
	g_ClientExports.pRenderPostOverlay();
}

void R_DrawRect(vec2_t position, vec2_t size, vec4_t color)
{
	// TODO: Implement.
}

void R_DrawText(font_t* font, vec2_t position, float scale, vec4_t color, const char* text)
{
	uint32_t fontEnd = FONT_FIRST_CHAR + FONT_CHARS;

	uint32_t len = strlen(text);

	float xOffset = 0.0f;
	float yOffset = 0.0f;

	struct {
		vec4_t color;
		vec2_t pos;
		vec2_t size;
		vec4_t uvs;
	} textInstance = { 
		.color = color, 
		.pos = NEW_VEC2(0.0f, 0.0f), 
		.size = NEW_VEC2(s_CommonData.width, s_CommonData.height),
		.uvs = NEW_VEC4(0.0f, 0.0f, 1.0f, 1.0f)
	};

	for (uint32_t i = 0; i < len; i++) {
		char c = text[i];

		if (c == '\n') {
			xOffset = 0.0f;
			yOffset -= 64.0f * scale;
		} 

		if(c < FONT_FIRST_CHAR || c > fontEnd) {
			continue;
		}

		struct glyph* cGlyph = &font->glyphs[c - FONT_FIRST_CHAR];

		textInstance.size = Vec2_Muls(cGlyph->size, scale);
		textInstance.pos = NEW_VEC2(position.x + (cGlyph->off.x + xOffset) * scale, position.y - (cGlyph->off.y + cGlyph->size.y - yOffset) * scale);
		textInstance.uvs = NEW_VEC4(cGlyph->uvs[0].x, cGlyph->uvs[0].y, cGlyph->uvs[1].x, cGlyph->uvs[1].y);

		Mesh_Draw(&s_MeshText, &textInstance);

		xOffset += cGlyph->advance;
	}
}

ivec2_t R_GetWindowSize()
{
	return NEW_IVEC2(s_CommonData.width, s_CommonData.height);
}

texture_t* R_GetMissingTexture()
{
	return &s_TextureMissing;
}

texture_t* R_GetWhiteTexture()
{
	return &s_TextureWhite;
}

texture_t* R_GetBlackTexture()
{
	return &s_TextureBlack;
}

material_t* R_GetWhiteMaterial()
{
	return &s_MaterialWhite;
}

mesh_t* R_GetCubeMesh()
{
	return &s_MeshCube;
}

view_t g_View;
