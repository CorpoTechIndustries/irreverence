#pragma once

#include <stdbool.h>

void IN_Init();
void IN_Update();
void IN_KeyEvent(int key, int down);
void IN_MouseEvent(int button, int down);
void IN_MotionEvent(float x, float y);

bool IN_IsKeyDown(int key);
bool IN_IsKeyPressed(int key);
bool IN_IsKeyUp(int key);

bool IN_IsMouseDown(int button);
bool IN_IsMousePressed(int button);
bool IN_IsMouseUp(int button);

void IN_GetMousePos(float* x, float* y);
void IN_GetMouseDelta(float* x, float* y);
