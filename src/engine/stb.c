#define STB_IMAGE_IMPLEMENTATION

#include <platform/memory.h>

#define STBI_MALLOC Sys_Malloc
#define STBI_REALLOC Sys_ReAlloc
#define STBI_FREE Sys_Free

#include <stb_image.h>
