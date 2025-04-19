#pragma once

#include <math/vec2.h>

#define FONT_FIRST_CHAR	32
#define FONT_CHARS 95

typedef struct font {
	struct glyph {
		vec2_t size;
		vec2_t off;
		vec2_t uvs[2]; // tex coords 
		float advance;
	} glyphs[FONT_CHARS];
} font_t;

void Font_Init(font_t* font, const char* path);
void Font_Destroy(font_t* font);
