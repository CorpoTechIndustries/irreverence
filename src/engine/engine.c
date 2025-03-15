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

	mesh_vertexmodel_t triVertices[] = {
		{ 0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	0.5f, 1.0f },
		{ 0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f },
		{ -0.5f, -0.5f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f }
	};
	uint32_t triIndices[] = { 0, 1, 2 };

	mesh_t triMesh;
	Mesh_InitModel(&triMesh, triVertices, sizeof(triVertices) / sizeof(mesh_vertexmodel_t), triIndices, sizeof(triIndices) / sizeof(uint32_t));

	mesh_instancemodel_t floorInstance = {
		.r = 0.75f,
		.g = 0.75f,
		.b = 0.75f,
		.a = 1.0f,
		.model = MAT4_IDENTITY
	};
	Mat4_Translate(&floorInstance.model, NEW_VEC3(0.0f, -1.0f, 0.0f));
	Mat4_Scale(&floorInstance.model, NEW_VEC3(10.0f, 1.0f, 10.0f));

	mesh_instancemodel_t snickInstance = {
		.r = 0.9f,
		.g = 0.9f,
		.b = 0.9f,
		.a = 1.0f,
		.model = MAT4_IDENTITY
	};

	Mat4_Translate(&snickInstance.model, NEW_VEC3(0.0f, 1.5f, -2.0f));
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

	uint32_t lid1 = Lighting_AddSpotlight(&lightParams);

	lightParams.color = NEW_VEC3(1.0f, 0.0f, 0.0f);

	g_View.position = NEW_VEC3(0.0f, 2.0f, 0.0f);

	float hi = 0.5f;
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
			
			if (lid1 != UINT32_MAX) {
				Lighting_RemoveSpotlight(lid1);
				lid1 = UINT32_MAX;
			}

			pointlight_data_t christParams = {
				.position = NEW_VEC3(-3.0f, 2.0f, -1.0f),
				.color = NEW_VEC3(1.0f, 0.0f, 0.0f),
				.brightness = 0.25f,
				.radius = 100.0f
			};
			
			Lighting_AddPointlight(&christParams);
			
			christParams.position = NEW_VEC3(3.0f, 2.0f, -1.0f);
			christParams.color = NEW_VEC3(0.0f, 1.0f, 0.0f);
			Lighting_AddPointlight(&christParams);
		}

		if (IN_IsKeyPressed(GLFW_KEY_R)) {
			hi += 0.05f;
			Lighting_SetSLightBrightness(lid1, hi);
		}

		Framebuffer_Bind(&testFramebuffer);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Shader_Bind(&testShader);
		Texture_Bind(R_GetWhiteTexture(), 0);
		Mesh_DrawModel(R_GetCubeMesh(), &floorInstance);

		Model_Draw(&testModel, &snickInstance, 0);

		Framebuffer_UnBind();
		Framebuffer_CopyTo(&testFramebuffer, NULL, false);

		glfwSwapBuffers(window);
	}

	Snd_UnloadSound(sound);
	Snd_DestroyStream(&stream);

	Mesh_Destroy(&triMesh);
	Shader_Destroy(&testShader);
	Texture_Destroy(&testTexture);

	Model_Destroy(&testModel);

	Snd_Destroy();
	Framebuffer_Destroy(&testFramebuffer);

	R_Destroy();

	glfwDestroyWindow(window);

	Sys_Close();

	return EXIT_SUCCESS;
}
