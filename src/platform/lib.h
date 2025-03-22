#pragma once

typedef void* dll_t;

dll_t Sys_OpenLibrary(const char* path);
void Sys_CloseLibrary(dll_t dll);
void* Sys_GetProcAddress(dll_t dll, const char* sym);
