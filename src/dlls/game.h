#pragma once

#include <public/engine.h>

extern "C" bool GameDLLInit(engine_functions_t* functions, game_exports_t* exports, int interface_version);
