#include <engine/engine.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include <engine/audio/audio.h>
#include <engine/audio/sound.h>
#include <engine/renderer/renderer.h>
#include <engine/renderer/shader.h>
#include <engine/renderer/mesh.h>
#include <engine/renderer/texture.h>
#include <engine/renderer/framebuffer.h>
#include <engine/log.h>
#include <engine/input.h>
#include <util/bitset.h>

#include <platform/sys.h>

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

	R_WindowUpdate(1280, 720); // NOTE: "De-Hardcode" it

	mesh_modelvertex_t triVertices[] = {
		{ 0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.5f, 1.0f },
		{ 0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f },
		{ -0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f }
	};
	uint32_t triIndices[] = { 0, 1, 2 };

	mesh_t triMesh;
	Mesh_InitModel(&triMesh, triVertices, sizeof(triVertices) / sizeof(mesh_modelvertex_t), triIndices, sizeof(triIndices) / sizeof(uint32_t));

	mesh_modelinstance_t triInstance = {
		.r = 0.7f,
		.g = 0.7f,
		.b = 0.7f,
		.a = 1.0f,
		.model = MAT4_IDENTITY
	};

	Mat4_Translate(&triInstance.model, NEW_VEC3(0.0f, 0.0f, -2.0f));

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

	while (!glfwWindowShouldClose(window)) {
		IN_Update();

		glfwPollEvents();

		prevTime = curTime;
		curTime = (float)glfwGetTime();
		frameTime = curTime - prevTime;

		R_DebugMoveUpdate();

		R_Present();

		if (IN_IsKeyPressed(GLFW_KEY_P)) {
			Snd_PlayStream(&stream);
		}

		Framebuffer_Bind(&testFramebuffer);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Shader_Bind(&testShader);
		Texture_Bind(R_GetMissingTexture(), 0);
		Mesh_DrawModel(R_GetCubeMesh(), &triInstance);

		Framebuffer_UnBind();
		Framebuffer_CopyTo(&testFramebuffer, NULL, false);

		glfwSwapBuffers(window);
	}

	Snd_UnloadSound(sound);
	Snd_DestroyStream(&stream);

	Mesh_Destroy(&triMesh);
	Shader_Destroy(&testShader);
	Texture_Destroy(&testTexture);

	Snd_Destroy();
	Framebuffer_Destroy(&testFramebuffer);

	R_Destroy();

	glfwDestroyWindow(window);

	Sys_Close();

	return EXIT_SUCCESS;
}
