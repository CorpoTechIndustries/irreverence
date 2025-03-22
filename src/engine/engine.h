#pragma once

#include <public/engine.h>
#include <platform/lib.h>

int Engine_Run(int argc, const char** argv);

void Engine_ResetTime();
float Engine_CurTime();
float Engine_FrameTime();

extern game_exports_t g_GameExports;
extern dll_t g_GameDLL;
