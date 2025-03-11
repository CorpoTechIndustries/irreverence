#include <engine/engine.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include <engine/renderer/shader.h>
#include <engine/renderer/mesh.h>
#include <engine/renderer/texture.h>
#include <engine/log.h>
#include <engine/input.h>

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

	glfwMakeContextCurrent(window);

	GLenum res = glewInit();

	if (res != GLEW_OK) {
		return EXIT_FAILURE;
	}

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
		.model = { 1.0f }
	};

	shader_t testShader;
	Shader_Init(&testShader, "Sigma", "assets/shaders/test.vert", "assets/shaders/test.frag");

	texture_t testTexture;
	Texture_InitFromImage(&testTexture, "assets/textures/scateleton.png", true, false);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	IN_Init();

	while (!glfwWindowShouldClose(window)) {
		IN_Update();

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		Shader_Bind(&testShader);
		Texture_Bind(&testTexture, 0);
		Mesh_DrawModel(&triMesh, &triInstance);

		glfwSwapBuffers(window);
	}

	Mesh_Destroy(&triMesh);
	Shader_Destroy(&testShader);
	Texture_Destroy(&testTexture);

	glfwDestroyWindow(window);

	Sys_Close();

	return EXIT_SUCCESS;
}
