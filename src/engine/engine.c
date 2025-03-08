#include <engine/engine.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>

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

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	return EXIT_SUCCESS;
}
