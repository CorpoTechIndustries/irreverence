#include <engine/input.h>

#include <platform/memory.h>

#include <GLFW/glfw3.h>

static bool s_KeysDown[GLFW_KEY_LAST];
static bool s_KeysPressed[GLFW_KEY_LAST];

static bool s_MouseDown[16];
static bool s_MousePressed[16];

static float s_fMouseX = 0.0f;
static float s_fMouseY = 0.0f;
static float s_fOldMouseX = 0.0f;
static float s_fOldMouseY = 0.0f;

void IN_Init()
{
	Sys_MemZero(s_KeysDown, sizeof(s_KeysDown));
	Sys_MemZero(s_KeysPressed, sizeof(s_KeysPressed));

	Sys_MemZero(s_MouseDown, sizeof(s_MouseDown));
	Sys_MemZero(s_MousePressed, sizeof(s_MousePressed));
}

void IN_Update()
{
	Sys_MemZero(s_KeysPressed, sizeof(s_KeysPressed));
	Sys_MemZero(s_MousePressed, sizeof(s_MousePressed));

	s_fOldMouseX = s_fMouseX;
	s_fOldMouseY = s_fMouseY;
}

void IN_KeyEvent(int key, int down)
{
	s_KeysDown[key] = down;

	if (down) {
		s_KeysPressed[key] = down;
	}
}

void IN_MouseEvent(int button, int down)
{
	s_MouseDown[button] = down;

	if (down) {
		s_MousePressed[button] = down;
	}
}

void IN_MotionEvent(float x, float y)
{
	s_fOldMouseX = s_fMouseX;
	s_fOldMouseY = s_fMouseY;

	s_fMouseX = x;
	s_fMouseY = y;
}

bool IN_IsKeyDown(int key)
{
	return s_KeysDown[key];
}

bool IN_IsKeyPressed(int key)
{
	return s_KeysPressed[key];
}

bool IN_IsKeyUp(int key)
{
	return !IN_IsKeyDown(key);
}

bool IN_IsMouseDown(int button)
{
	return s_MouseDown[button];
}

bool IN_IsMousePressed(int button)
{
	return s_MousePressed[button];
}

bool IN_IsMouseUp(int button)
{
	return !IN_IsMouseDown(button);
}

void IN_GetMousePos(float* x, float* y)
{
	if (x) {
		*x = s_fMouseX;
	}

	if (y) {
		*y = s_fMouseY;
	}
}

void IN_GetMouseDelta(float* x, float* y)
{
	if (x) {
		*x = (s_fOldMouseX - s_fMouseX);
	}

	if (y) {
		*y = (s_fOldMouseY - s_fMouseY);
	}
}
