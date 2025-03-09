#include <engine/engine.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include <engine/renderer/shader.h>
#include <engine/renderer/mesh.h>
#include <engine/renderer/texture.h>

int Engine_Run(int argc, const char** argv)
{
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

	while (!glfwWindowShouldClose(window)) {
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

	return EXIT_SUCCESS;
}
