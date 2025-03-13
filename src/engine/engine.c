#include <engine/engine.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

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

	while (!glfwWindowShouldClose(window)) {
		IN_Update();

		glfwPollEvents();
		
		R_DebugMoveUpdate();

		R_Present();

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

	Mesh_Destroy(&triMesh);
	Shader_Destroy(&testShader);
	Texture_Destroy(&testTexture);
	Framebuffer_Destroy(&testFramebuffer);

	R_Destroy();

	glfwDestroyWindow(window);

	Sys_Close();

	return EXIT_SUCCESS;
}
