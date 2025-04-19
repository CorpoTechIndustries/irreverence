#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include <platform/memory.h>

#define STBI_MALLOC Sys_Malloc
#define STBI_REALLOC Sys_ReAlloc
#define STBI_FREE Sys_Free

#define STBTT_malloc(x,u) ((void)(u), Sys_Malloc(x))
#define STBTT_free(x,u) ((void)(u), Sys_Free(x))

#include <stb_image.h>
#include <stb_truetype.h>
