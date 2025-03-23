#pragma once

#ifdef PLATFORM_WINDOWS
	#define DLL_EXPORT __declspec(dllexport)
#elif PLATFORM_LINUX
	#define DLL_EXPORT
#endif
