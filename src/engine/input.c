#include <engine/input.h>

#include <platform/memory.h>

#include <GLFW/glfw3.h>

static int s_KeysDown[GLFW_KEY_LAST];
static int s_KeysPressed[GLFW_KEY_LAST];

static int s_MouseDown[16];
static int s_MousePressed[16];

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

int IN_IsKeyDown(int key)
{
	return s_KeysDown[key];
}

int IN_IsKeyPressed(int key)
{
	return s_KeysPressed[key];
}

int IN_IsKeyUp(int key)
{
	return !IN_IsKeyDown(key);
}

int IN_IsMouseDown(int button)
{
	return s_MouseDown[button];
}

int IN_IsMousePressed(int button)
{
	return s_MousePressed[button];
}

int IN_IsMouseUp(int button)
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
