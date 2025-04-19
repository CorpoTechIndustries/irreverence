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
#include <engine/client.h>
#include <engine/server.h>

#include <platform/sys.h>
#include <platform/lib.h>
#include <platform/udp.h>
#include <util/endian.h>

#include <public/engine.h>

#include <platform/memory.h>

#include <engine/net_messages.h>

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

	bool show_demo = false;

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

	float hi = 0.5f;
	float nextTick = 0.0f;
	float tickRate = 1.0f/60.0f;

	g_GameExports.pGameInit();

	#ifndef ENGINE_DEDICATED
	g_ClientExports.pInit();
	#endif

	CL_Init(&cl);
	SV_Init(&sv);

	CVar_SetInt("sv_maxplayers", 1);

	SV_Host(&sv);

	while (!glfwWindowShouldClose(window)) {
		IN_Update();

		CL_Update(&cl);
		SV_Update(&sv);

		glfwPollEvents();

		if (IN_IsKeyPressed(GLFW_KEY_P)) {
			CL_Connect(&cl, "localhost", 27015);
		}

		if (IN_IsKeyPressed(GLFW_KEY_O)) {
			CL_NetSend(&cl, CLC_NOP, NULL, 0, true);
		}

		if (IN_IsKeyPressed(GLFW_KEY_I)) {
			SV_NetSend(&sv, sv.clients + 0, SVC_NOP, NULL, 0, true);
		}

		if (IN_IsKeyPressed(GLFW_KEY_Q)) {
			CL_Disconnect(&cl);
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		igNewFrame();

		if (show_demo) {
			igShowDemoWindow(&show_demo);
		}

		prevTime = curTime;
		curTime = (float)glfwGetTime();
		frameTime = curTime - prevTime;

		if (nextTick < curTime) {
			Phys_Update();
			nextTick = curTime + tickRate;
		}

		R_DebugMoveUpdate();

		R_Present();

		igRender();

		ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

		glfwSwapBuffers(window);
	}

	CL_Close(&cl);
	SV_Close(&sv);

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

	funcs.pRegisterCVar = CVar_Register;
	funcs.pGetCVar = CVar_Get;

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

static void draw_debug_cube(vec3_t origin, vec3_t size, vec4_t color)
{
	mesh_t* cube = R_GetCubeMesh();

	mesh_instancemodel_t in;
	in.model = MAT4_IDENTITY;
	in.r = color.x;
	in.g = color.y;
	in.b = color.z;
	in.a = color.w;

	Mat4_Translate(&in.model, origin);
	Mat4_Scale(&in.model, size);

	Mesh_Draw(cube, &in);
}

static bool setup_client()
{
	client_functions_t funcs;

	funcs.pMessage = log_info_wrapper;
	funcs.pWarning = log_warning_wrapper;
	funcs.pError = log_error_wrapper;
	funcs.pFatal = log_fatal_wrapper;

	funcs.pRegisterCVar = CVar_Register;
	funcs.pGetCVar = CVar_Get;

	funcs.pDebugDrawCube = draw_debug_cube;

#ifdef PLATFORM_LINUX
	const char* lib_name = "bin/client.so";
#elif PLATFORM_WINDOWS
	const char* lib_name = "bin/client.dll";
#endif

	g_ClientDLL = Sys_OpenLibrary(lib_name);

	if (!g_ClientDLL) {
		const char* err = dlerror();
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
