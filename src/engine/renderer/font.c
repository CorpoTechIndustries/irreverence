#include <engine/renderer/font.h>

#include <engine/log.h>

#include <util/buffer.h>
#include <platform/path.h>
#include <platform/memory.h>

#include <stb_truetype.h>
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h> // TODO: For testing, remove this later

#define FONT_ATLAS_WIDTH 1024 // shouldn't these be dynamic??
#define FONT_ATLAS_HEIGHT 1024

#define FONT_SIZE 64.0f

void Font_Init(font_t* font, const char* path)
{
	Sys_MemZero(font->glyphs, sizeof(font->glyphs));

	size_t size = 0;
	char* data = Sys_PathReadFile(path, &size);

	if (!data) {
		LOG_ERROR("Failed to read Font file \"%s\"", path);
		return;
	}

	stbtt_fontinfo stbFont = { 0 };

	if (!stbtt_InitFont(&stbFont, data, 0)) {
		LOG_ERROR("Failed to load Font \"%s\"", path);
		return;
	}

	uint8_t* fontAtlasData = Sys_Malloc(FONT_ATLAS_WIDTH * FONT_ATLAS_HEIGHT);

	stbtt_pack_context stbPack;
	stbtt_packedchar stbPackedChars[FONT_CHARS];
	stbtt_aligned_quad stbAlignedQuads[FONT_CHARS];

	stbtt_PackBegin(&stbPack, fontAtlasData, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT, 0, 1, NULL);

	stbtt_PackFontRange(&stbPack, data, 0, FONT_SIZE, FONT_FIRST_CHAR, FONT_CHARS, stbPackedChars);

	stbtt_PackEnd(&stbPack);

	for (char i = 0; i < FONT_CHARS; i++) {
		float unused;
		
		stbtt_packedchar* pchar = &stbPackedChars[i];
		stbtt_GetPackedQuad(stbPackedChars, FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT, i, &unused, &unused, &stbAlignedQuads[i], 0);
		stbtt_aligned_quad* aquad = &stbAlignedQuads[i];

		struct glyph* fontGlyph = &font->glyphs[i];

		fontGlyph->uvs[0] = NEW_VEC2(aquad->s0, 1.0f - aquad->t1);
		fontGlyph->uvs[1] = NEW_VEC2(aquad->s1, 1.0f - aquad->t0);

		fontGlyph->advance = pchar->xadvance;
		fontGlyph->size = NEW_VEC2(pchar->x1 - pchar->x0, pchar->y1 - pchar->y0);
		fontGlyph->off = NEW_VEC2(pchar->xoff, pchar->yoff);
	}
	
	stbi_write_png("nigga.png", FONT_ATLAS_WIDTH, FONT_ATLAS_HEIGHT, 1, fontAtlasData, FONT_ATLAS_WIDTH);

	Sys_Free(fontAtlasData);
}

void Font_Destroy(font_t* font)
{

}
