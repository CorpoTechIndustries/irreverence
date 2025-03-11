#pragma once

void IN_Init();
void IN_Update();
void IN_KeyEvent(int key, int down);
void IN_MouseEvent(int button, int down);
void IN_MotionEvent(float x, float y);

int IN_IsKeyDown(int key);
int IN_IsKeyPressed(int key);
int IN_IsKeyUp(int key);

int IN_IsMouseDown(int button);
int IN_IsMousePressed(int button);
int IN_IsMouseUp(int button);

void IN_GetMousePos(float* x, float* y);
void IN_GetMouseDelta(float* x, float* y);
