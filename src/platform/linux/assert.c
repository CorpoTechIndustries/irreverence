#include <platform/assert.h>
#include <platform/thread.h>
#include <execinfo.h>

// NOTE: The execinfo funcs use malloc internally so using free here instead
// of platform defined ones incase we use a custom allocator later on.
#include <malloc.h>

#include <platform/io.h>

#define MAX_BACKTRACE 100

void __Internal__Throw__Assert(const char* reason, const char* filename, size_t line, const char* function_name)
{
	static void* backtraces[MAX_BACKTRACE];

	int pointerCount = backtrace(backtraces, MAX_BACKTRACE);

	char** strs = backtrace_symbols(backtraces, pointerCount);

	if (!strs) {
		return;
	}

	thread_t curThread = Sys_ThreadCurrent();

	if (!Sys_ThreadIsMain()) {
		Sys_Printf("Assert Failed in %s:%d (%s) (thread %zu)",
			filename, line, function_name, curThread, reason
		);
	} else {
		Sys_Printf("Assert Failed in %s:%d (%s) (main thread): %s\n",
			filename, line, function_name, reason
		);
	}

	// Ignore our current function
	for (int i = 1; i < pointerCount; i++) {
		Sys_Printf("    %s\n", strs[i]);
	}

	free(strs);
}
