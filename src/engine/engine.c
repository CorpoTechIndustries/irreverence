#include <engine/engine.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include <engine/audio/audio.h>
#include <engine/audio/sound.h>
#include <engine/renderer/renderer.h>
#include <engine/renderer/lighting.h>
#include <engine/renderer/shader.h>
#include <engine/renderer/mesh.h>
#include <engine/renderer/model.h>
#include <engine/renderer/texture.h>
#include <engine/renderer/framebuffer.h>
#include <engine/physics/physics.h>
#include <engine/log.h>
#include <engine/input.h>
#include <engine/edict.h>

#include <platform/sys.h>
#include <platform/lib.h>

#include <public/engine.h>

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_REPEAT) {
		return;
	}

	IN_KeyEvent(key, action == GLFW_PRESS);
}

static void button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_REPEAT) {
		return;
	}

	IN_MouseEvent(button, action == GLFW_PRESS);
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	IN_MotionEvent((float)xpos, (float)ypos);
}

static void resize_callback(GLFWwindow* window, int width, int height)
{
	R_WindowUpdate(width, height);
}

static float startTime = 0.0f;
static float curTime = 0.0f;
static float prevTime = 0.0f;
static float frameTime = 0.0f;

game_exports_t g_GameExports = { 0 };
dll_t g_GameDLL = NULL;

void Engine_ResetTime()
{
	prevTime = curTime = startTime = (float)glfwGetTime();
}

float Engine_CurTime()
{
	return curTime - startTime;
}

float Engine_FrameTime()
{
	return frameTime;
}

static bool setup_game();
static bool setup_client();

int Engine_Run(int argc, const char** argv)
{
	Sys_Init();

	if (!glfwInit()) {
		return EXIT_FAILURE;
	}
	atexit(glfwTerminate);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Irreverence", NULL, NULL);

	if (!window) {
		return EXIT_FAILURE;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetWindowSizeCallback(window, resize_callback);

	glfwMakeContextCurrent(window);

	if (!R_Init()) {
		return EXIT_FAILURE;
	}

	if (!Snd_Init()) {
		return EXIT_FAILURE;
	}

	if (!Phys_Init()) {
		return EXIT_FAILURE;
	}

	if (!ED_Init()) {
		return EXIT_FAILURE;
	}

	if (!setup_game()) {
		return EXIT_FAILURE;
	}

	if (!setup_client()) {
		return EXIT_FAILURE;
	}

	R_WindowUpdate(1280, 720); // NOTE: "De-Hardcode" it

	mesh_vertexmodel_t triVertices[] = {
		{ 0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.5f, 1.0f },
		{ 0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f },
		{ -0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f }
	};
	uint32_t triIndices[] = { 0, 1, 2 };

	mesh_t triMesh;
	Mesh_InitModel(&triMesh, triVertices, sizeof(triVertices) / sizeof(mesh_vertexmodel_t), triIndices, sizeof(triIndices) / sizeof(uint32_t));

	mesh_instancemodel_t snickInstance = {
		.r = 0.9f,
		.g = 0.9f,
		.b = 0.9f,
		.a = 1.0f,
		.model = MAT4_IDENTITY
	};

	Mat4_Translate(&snickInstance.model, NEW_VEC3(2.0f, 1.5f, -2.0f));
	Mat4_Scale(&snickInstance.model, NEW_VEC3S(0.2f));

	shader_t testShader;
	Shader_InitRaster(&testShader, "Sigma", "assets/shaders/test.vert", "assets/shaders/test.frag");

	texture_t testTexture;
	Texture_Init(&testTexture, "assets/textures/scateleton.png", true, false);

	framebuffer_attachment_t testAttachments[] = {
		{ .format = GL_RGB8, .type = GL_UNSIGNED_BYTE }
	};
	framebuffer_t testFramebuffer;
	Framebuffer_Init(&testFramebuffer, 1280, 720, 0, testAttachments, 1, NULL);

	IN_Init();

	sound_file_t soundFile;

	if (!Snd_LoadSoundFromFile(&soundFile, "assets/sounds/test.wav")) {
		LOG_ERROR("WHAT THE FUCK?");
	}

	sound_t sound =	Snd_LoadSound(&soundFile);

	Snd_FreeFile(&soundFile);

	sound_stream_t stream;

	Snd_CreateStream(&stream);

	Snd_SetStreamSound(&stream, sound);

	model_t testModel;
	Model_Init(&testModel, "assets/models/snickerer.obj");
	spotlight_data_t lightParams = {
		.position = NEW_VEC3(0.0f, 5.0f, -1.0f),
		.direction = NEW_VEC3(0.0f, -1.0f, 0.0f),
		.color = NEW_VEC3S(1.0f),
		.cutoff = 45.0f,
		.outerCutoff = 60.0f,
		.brightness = 0.5f
	};

	uint32_t lid1 = Light_AddSpotlight(&lightParams);

	lightParams.color = NEW_VEC3(1.0f, 0.0f, 0.0f);

	g_View.position = NEW_VEC3(0.0f, 2.0f, 0.0f);


	quat_t coobeStartQuat = QUAT_IDENTITY;
	Quat_AxisAngle(45.0f, NEW_VEC3S(1.0f), &coobeStartQuat);

	physobj_t* coobe = Phys_AddCube(NEW_VEC3(-2.0f, 20.0f, -3.0f), coobeStartQuat, NEW_VEC3S(0.5f), PHYS_TYPE_DYNAMIC, PHYS_LAYER_MOVING);
	mesh_instancemodel_t coobeInstance = {
		.r = 1.0f,
		.g = 0.5f,
		.b = 0.5f,
		.a = 1.0f,
		.model = MAT4_IDENTITY
	};

	physobj_t* floor = Phys_AddCube(NEW_VEC3(0.0f, -1.0f, 0.0f), QUAT_IDENTITY, NEW_VEC3(10.0f, 1.0f, 10.0f), PHYS_TYPE_STATIC, PHYS_LAYER_NONMOVING);
	mesh_instancemodel_t floorInstance = {
		.r = 0.75f,
		.g = 0.75f,
		.b = 0.75f,
		.a = 1.0f,
		.model = MAT4_IDENTITY
	};
	Mat4_Translate(&floorInstance.model, NEW_VEC3(0.0f, -1.0f, 0.0f));
	Mat4_Scale(&floorInstance.model, NEW_VEC3(10.0f, 1.0f, 10.0f));

	float hi = 0.5f;
	float nextTick = 0.0f;
	float tickRate = 1.0f/60.0f;

	g_GameExports.pGameInit();

	while (!glfwWindowShouldClose(window)) {
		IN_Update();

		glfwPollEvents();

		prevTime = curTime;
		curTime = (float)glfwGetTime();
		frameTime = curTime - prevTime;

		if (nextTick < curTime) {
			Phys_Update(frameTime);

			nextTick = curTime + tickRate;
		}

		for (size_t i = 0; i < EDICT_MAX_COUNT; i++) {
			edict_t* e = g_pEdicts + i;

			if (e->free) {
				continue;
			}

			g_GameExports.pThink(e);
		}

		R_DebugMoveUpdate();

		R_Present();

		if (IN_IsKeyPressed(GLFW_KEY_P)) {
			Snd_PlayStream(&stream);

			if (lid1 != UINT32_MAX) {
				Light_RemoveSpotlight(lid1);
				lid1 = UINT32_MAX;
			}

			pointlight_data_t christParams = {
				.position = NEW_VEC3(-3.0f, 2.0f, -1.0f),
				.color = NEW_VEC3(1.0f, 0.0f, 0.0f),
				.brightness = 0.25f,
				.radius = 100.0f
			};

			Light_AddPointlight(&christParams);

			christParams.position = NEW_VEC3(3.0f, 2.0f, -1.0f);
			christParams.color = NEW_VEC3(0.0f, 1.0f, 0.0f);
			Light_AddPointlight(&christParams);
		}

		if (IN_IsKeyPressed(GLFW_KEY_R)) {
			hi += 0.05f;
			Light_SetSLightBrightness(lid1, hi);
		}

		Framebuffer_Bind(&testFramebuffer);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Shader_Bind(&testShader);
		Texture_Bind(R_GetWhiteTexture(), 0);
		Mesh_DrawModel(R_GetCubeMesh(), &floorInstance);

		Model_Draw(&testModel, &snickInstance, 0);

		coobeInstance.model = MAT4_IDENTITY;
		Mat4_Translate(&coobeInstance.model, coobe->position);
		Mat4_Scale(&coobeInstance.model, NEW_VEC3S(0.5f));

		float coobeAngle = 0.0f;
		vec3_t coobeAxis = VEC3_ZERO;
		Quat_GetAxisAngle(&coobe->rotation, &coobeAngle, &coobeAxis);
		Mat4_Rotate(&coobeInstance.model, coobeAngle, coobeAxis);

		Mesh_DrawModel(R_GetCubeMesh(), &coobeInstance);

		Framebuffer_UnBind();
		Framebuffer_CopyTo(&testFramebuffer, NULL, false);

		glfwSwapBuffers(window);
	}

	g_GameExports.pGameClose();

	if (g_GameDLL) {
		Sys_CloseLibrary(g_GameDLL);
		g_GameDLL = NULL;
	}

	Snd_UnloadSound(sound);
	Snd_DestroyStream(&stream);

	Mesh_Destroy(&triMesh);
	Shader_Destroy(&testShader);
	Texture_Destroy(&testTexture);

	Model_Destroy(&testModel);
	Framebuffer_Destroy(&testFramebuffer);

	Phys_Destroy();

	Snd_Destroy();

	R_Destroy();

	glfwDestroyWindow(window);

	Sys_Close();

	return EXIT_SUCCESS;
}

static void log_info_wrapper(const char* fmt, ...)
{
	va_list lst;

	va_start(lst, fmt);
		Log_Messagev(LOG_LEVEL_INFO, fmt, lst);
	va_end(lst);
}

static void log_warning_wrapper(const char* fmt, ...)
{
	va_list lst;

	va_start(lst, fmt);
		Log_Messagev(LOG_LEVEL_WARNING, fmt, lst);
	va_end(lst);
}

static void log_error_wrapper(const char* fmt, ...)
{
	va_list lst;

	va_start(lst, fmt);
		Log_Messagev(LOG_LEVEL_ERROR, fmt, lst);
	va_end(lst);
}

static void log_fatal_wrapper(const char* fmt, ...)
{
	va_list lst;

	va_start(lst, fmt);
		Log_Messagev(LOG_LEVEL_FATAL, fmt, lst);
	va_end(lst);
}

static bool setup_game()
{
	engine_functions_t funcs;

	funcs.pMessage = log_info_wrapper;
	funcs.pWarning = log_warning_wrapper;
	funcs.pError = log_error_wrapper;
	funcs.pFatal = log_fatal_wrapper;

	funcs.pEdictCreate = ED_Create;
	funcs.pEdictCreateName = ED_CreateByName;
	funcs.pEdictGet = ED_Get;
	funcs.pEdictFree = ED_Free;

#ifdef PLATFORM_LINUX
	const char* lib_name = "bin/game.so";
#elif PLATFORM_WINDOWS
	const char* lib_name = "bin/game.dll";
#endif

	g_GameDLL = Sys_OpenLibrary(lib_name);

	if (!g_GameDLL) {
		LOG_FATAL("game.so could not be opened");
		return false;
	}

	bool(*init)(engine_functions_t* funcs, game_exports_t* exports, int engine_api_version);

	init = Sys_GetProcAddress(g_GameDLL, "GameDLLInit");

	if (!init) {
		LOG_FATAL("GameDLLInit symbol could not be found");
		return false;
	}

	if (!init(&funcs, &g_GameExports, ENGINE_INTERFACE_VERSION)) {
		LOG_FATAL("game.so does not support engine interface version");
		return false;
	}

	return true;
}

static bool setup_client()
{
	return true;
}
