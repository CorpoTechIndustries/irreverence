#pragma once

#include <engine/renderer/texture.h>

#include <stdbool.h>

bool R_Init();
void R_Destroy();
void R_WindowUpdate(int width, int height);

void R_Present();

texture_t* R_GetMissingTexture();
texture_t* R_GetWhiteTexture();
texture_t* R_GetBlackTexture();
