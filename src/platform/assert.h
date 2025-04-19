#pragma once

#include <stddef.h>

void __Internal__Throw__Assert(const char* reason, const char* filename, size_t line, const char* function_name);

// TODO: Figure out all the windows ones eventually
#ifdef __GNUC__

#define ASSERT_TRAP __builtin_trap()

#endif

#define AssertMessage(cond, message) \
if (!(cond)) { \
	__Internal__Throw__Assert(message, __FILE__, __LINE__, __FUNCTION__); \
	ASSERT_TRAP; \
}

#define Assert(cond) AssertMessage(cond, #cond)
