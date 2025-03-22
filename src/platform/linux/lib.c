#include <platform/lib.h>

#include <dlfcn.h>

dll_t Sys_OpenLibrary(const char* path)
{
	return dlopen(path, RTLD_NOW);
}

void Sys_CloseLibrary(dll_t dll)
{
	dlclose(dll);
}

void* Sys_GetProcAddress(dll_t dll, const char* sym)
{
	return dlsym(dll, sym);
}
