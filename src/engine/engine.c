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
#include <engine/renderer/animation.h>
#include <engine/renderer/texture.h>
#include <engine/renderer/framebuffer.h>
#include <engine/physics.h>
#include <engine/log.h>
#include <engine/input.h>
#include <engine/edict.h>
#include <engine/net.h>
#include <engine/cvar.h>

#include <platform/sys.h>
#include <platform/lib.h>
#include <platform/udp.h>
#include <util/endian.h>

#include <public/engine.h>

#include <platform/memory.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

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

#ifndef ENGINE_DEDICATED
client_exports_t g_ClientExports = { 0 };
dll_t g_ClientDLL = NULL;
#endif

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

#ifndef ENGINE_DEDICATED
static bool setup_client();
#endif

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

	igCreateContext(NULL);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(NULL);

	ImGuiIO* io = igGetIO_Nil();
	io->IniFilename = NULL;
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	bool show_demo = true;

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

	if (!Net_Init()) {
		return EXIT_FAILURE;
	}

	if (!setup_game()) {
		return EXIT_FAILURE;
	}

	#ifndef ENGINE_DEDICATED

	if (!setup_client()) {
		return EXIT_FAILURE;
	}

	#endif

	R_WindowUpdate(1280, 720); // NOTE: "De-Hardcode" it

	IN_Init();

	shader_t testShader;
	Shader_InitRaster(&testShader, "Sigma", "assets/shaders/test.vert", "assets/shaders/test.frag");

	texture_t testTexture;
	Texture_Init(&testTexture, "assets/textures/scateleton.png", true, false);

	framebuffer_attachment_t testAttachments[] = {
		{ .format = GL_RGB8, .type = GL_UNSIGNED_BYTE }
	};
	framebuffer_t testFramebuffer;
	Framebuffer_Init(&testFramebuffer, 1280, 720, 0, testAttachments, 1, NULL);

	sound_file_t soundFile;

	if (!Snd_LoadSoundFromFile(&soundFile, "assets/sounds/test.wav")) {
		LOG_ERROR("WHAT THE FUCK?");
	}

	sound_t sound =	Snd_LoadSound(&soundFile);

	Snd_FreeFile(&soundFile);

	sound_stream_t stream;

	Snd_CreateStream(&stream);

	Snd_SetStreamSound(&stream, sound);

	pointlight_data_t lightParams = {
		.position = NEW_VEC3(0.0f, 4.0f, -1.0f),
		.color = NEW_VEC3S(1.0f),
		.brightness = 0.5f
	};
	uint32_t lid1 = Light_AddPointlight(&lightParams);

	g_View.position = NEW_VEC3(0.0f, 2.0f, 0.0f);

	model_t mapModel;
	Model_Init(&mapModel, "assets/models/mapthing.obj");
	mesh_instancemodel_t mapInstance = {
		.r = 0.75f,
		.g = 0.75f,
		.b = 0.75f,
		.a = 1.0f,
		.model = MAT4_IDENTITY
	};
	Mat4_Translate(&mapInstance.model, NEW_VEC3(7.25f, 0.0f, 0.0f));
	Mat4_Scale(&mapInstance.model, NEW_VEC3S(0.5f));

	model_t animModel;
	Model_Init(&animModel, "assets/models/dancing_vampire.dae");
	mesh_instancemodel_t animInstance = {
		.r = 0.5f,
		.g = 0.5f,
		.b = 0.5f,
		.a = 1.0f,
		.model = MAT4_IDENTITY
	};
	Mat4_Translate(&animInstance.model, NEW_VEC3(0.0f, -3.25f, -5.0f));
	Mat4_Scale(&animInstance.model, NEW_VEC3S(0.02f));

	quat_t coobeStartQuat = QUAT_IDENTITY;
	Quat_AxisAngle(45.0f, NEW_VEC3S(1.0f), &coobeStartQuat);
	physobj_t* coobe = Phys_AddCube(NEW_VEC3(-2.0f, 20.0f, -3.0f), coobeStartQuat, NEW_VEC3S(0.5f), PHYS_TYPE_DYNAMIC, PHYS_LAYER_MOVING);

	physobj_t* floor = Phys_AddCube(NEW_VEC3(0.0f, -7.0f, 0.0f), QUAT_IDENTITY, NEW_VEC3(100.0f, 1.0f, 100.0f), PHYS_TYPE_STATIC, PHYS_LAYER_NONMOVING);

	float hi = 0.5f;
	float nextTick = 0.0f;
	float tickRate = 1.0f/60.0f;

	g_GameExports.pGameInit();

	Net_Setup(false);

	net_address_t local = Net_LocalAddress(27015);

	while (!glfwWindowShouldClose(window)) {
		IN_Update();

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		igNewFrame();

		if (show_demo) {
			igShowDemoWindow(&show_demo);
		}

		{
			int n = 0;

			net_address_t from;
			int server_read = Net_ReadPacket(NET_SERVER, &n, sizeof(n), &from);

			if (server_read > -1) {
				uint16_t port;
				const char* address = Net_AddressToString(from, &port);
				LOG_INFO("Got %d from client %s:%u", n, address, port);

				Net_SendPacket(NET_SERVER, &n, sizeof(n), from);
			}
		}

		{
			int n = 0;

			net_address_t from;
			int client_read = Net_ReadPacket(NET_CLIENT, &n, sizeof(n), &from);

			uint16_t port;
			const char* address = Net_AddressToString(from, &port);

			if (client_read > -1) {
				LOG_INFO("Got %d from the server(%s:%u)", n, address, port);
			}
		}

		if (IN_IsKeyPressed(GLFW_KEY_5)) {
			int n = 125;

			Net_SendLoopbackPacket(NET_CLIENT, &n, sizeof(n), local);
		}

		prevTime = curTime;
		curTime = (float)glfwGetTime();
		frameTime = curTime - prevTime;

		if (nextTick < curTime) {
			Phys_Update();
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
				Light_RemovePointlight(lid1);
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
		mesh_instancemodel_t coobeInstance = {
			.r = 1.0f,
			.g = 0.5f,
			.b = 0.5f,
			.a = 1.0f
		};
		coobeInstance.model = MAT4_IDENTITY;
		Mat4_Scale(&coobeInstance.model, NEW_VEC3S(0.5f));
		Mat4_Translate(&coobeInstance.model, coobe->position);

		float coobeAngle = 0.0f;
		vec3_t coobeAxis = VEC3_ZERO;
		Quat_GetAxisAngle(&coobe->rotation, &coobeAngle, &coobeAxis);
		Mat4_Rotate(&coobeInstance.model, coobeAngle, coobeAxis);
		Mesh_AddInstance(R_GetCubeMesh(), &coobeInstance);

		Model_AddInstance(&mapModel, &mapInstance);
		Model_DrawInstances(&mapModel);

		Texture_Bind(R_GetWhiteTexture(), 0);
		Mesh_DrawInstances(R_GetCubeMesh());

		Model_Draw(&animModel, &animInstance, 0);

		Framebuffer_UnBind();
		Framebuffer_CopyTo(&testFramebuffer, NULL, false);

		Mesh_ClearInstances(R_GetCubeMesh());
		Model_ClearInstances(&mapModel);

		igRender();

		ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

		glfwSwapBuffers(window);
	}

	g_GameExports.pGameClose();

	if (g_GameDLL) {
		Sys_CloseLibrary(g_GameDLL);
		g_GameDLL = NULL;
	}

	#ifndef ENGINE_DEDICATED

	g_ClientExports.pClose();

	if (g_ClientDLL) {
		Sys_CloseLibrary(g_ClientDLL);
		g_ClientDLL = NULL;
	}

	#endif

	Snd_UnloadSound(sound);
	Snd_DestroyStream(&stream);

	Shader_Destroy(&testShader);
	Texture_Destroy(&testTexture);

	Model_Destroy(&mapModel);
	Model_Destroy(&animModel);
	Framebuffer_Destroy(&testFramebuffer);

	Net_Close();

	ED_Close();

	Phys_Destroy();

	Snd_Destroy();

	R_Destroy();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(NULL);

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
		LOG_FATAL("game lib could not be opened");
		return false;
	}

	game_init_fn_t init = Sys_GetProcAddress(g_GameDLL, "GameDLLInit");

	if (!init) {
		LOG_FATAL("GameDLLInit symbol could not be found");
		return false;
	}

	if (!init(&funcs, &g_GameExports, ENGINE_INTERFACE_VERSION)) {
		LOG_FATAL("game lib does not support engine interface version");
		return false;
	}

	return true;
}

#ifndef ENGINE_DEDICATED

static bool setup_client()
{
	client_functions_t funcs;

	funcs.pMessage = log_info_wrapper;
	funcs.pWarning = log_warning_wrapper;
	funcs.pError = log_error_wrapper;
	funcs.pFatal = log_fatal_wrapper;

#ifdef PLATFORM_LINUX
	const char* lib_name = "bin/client.so";
#elif PLATFORM_WINDOWS
	const char* lib_name = "bin/client.dll";
#endif

	g_ClientDLL = Sys_OpenLibrary(lib_name);

	if (!g_ClientDLL) {
		LOG_FATAL("client library could not be opened!");
		return false;
	}

	client_init_fn_t init = Sys_GetProcAddress(g_ClientDLL, "ClientInit");

	if (!init) {
		LOG_FATAL("ClientInit symbol could not be found");
		return false;
	}

	if (!init(&funcs, &g_ClientExports, CLIENT_INTERFACE_VERSION)) {
		LOG_FATAL("client lib does not support client interface version");
		return false;
	}

	return true;
}

#endif
