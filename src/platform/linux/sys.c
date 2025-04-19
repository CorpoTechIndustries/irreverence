#include <platform/sys.h>
#include <platform/io.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <platform/thread.h>
#include <pthread.h>
#ifndef __USE_POSIX
	#define __USE_POSIX
#endif

#ifndef __GNU_SOURCE
	#define __GNU_SOURCE
#endif

#ifndef __USE_GNU
	#define __USE_GNU
#endif

#ifndef __USE_MISC
	#define __USE_MISC
#endif

#ifndef __USE_POSIX199309
	#define __USE_POSIX199309
#endif

#include <memory.h>
#include <signal.h>
#include <execinfo.h>
#include <ucontext.h>
#include <unistd.h>
#include <dlfcn.h>

#include <malloc.h>

#define MAX_BACKTRACE 100

static const char* register_names[NGREG] = {
	"R8", "R9", "R10", "R11", "R12",
	"R13", "R14", "R15", "RDI", "RSI",
	"RBP", "RBX", "RDX", "RAX", "RCX",
	"RSP", "RIP", "EFL", "CSGSFS", "ERR",
	"TRAPNO", "OLDMASK", "CR2"
};

static void segv_handler(int signal, siginfo_t* info, void* ptr)
{
	ucontext_t* context = (ucontext_t*)ptr;

	Sys_Print("Segmentation Fault!\n");

	void* ip = (void*)context->uc_mcontext.gregs[REG_RIP];
	void** bp = (void**)context->uc_mcontext.gregs[REG_RBP];
	static void* backtraces[MAX_BACKTRACE];

	int ticker = 0;
	Sys_Print(" Register State:\n    ");
	for (size_t i = 0; i < NGREG; i++) {
		Sys_Printf("Reg[%s] = %#016lx ", register_names[i], context->uc_mcontext.gregs[i]);

		if (++ticker > 3) {
			Sys_Print("\n    ");
			ticker = 0;
		}
	}

	Sys_Print("\n");

	Sys_Print("  Stack Trace:\n");

	Dl_info dlinfo;
	while (ip && bp) {
		if (!dladdr(ip, &dlinfo)) {
			break;
		}

		const char* symbolName = dlinfo.dli_sname;

		Sys_Printf("    %p <%s+%#016lx> (%s)\n",
			ip, symbolName, (unsigned long)ip - (unsigned long)dlinfo.dli_saddr,
			dlinfo.dli_fname
		);

		ip = bp[1];
		bp = (void**)bp[0];
	}

	_exit(-1);
}

void Sys_Init()
{
	struct sigaction act;

	act.sa_handler = (sighandler_t)segv_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGSEGV, &act, NULL);

	Sys_ThreadInit();
	Sys_IOInit();
}

void Sys_Close()
{
	Sys_IOClose();
}

static void _sleep(struct timespec t)
{
	struct timespec remainder;

	if (syscall(SYS_nanosleep, &t, &remainder)) {
		_sleep(remainder);
	}
}

void Sys_Sleep(size_t ms)
{
	struct timespec t;

	t.tv_sec = ms / 1000;
	t.tv_nsec = (ms % 1000) * 1000000;

	_sleep(t);
}

void Sys_Exit(int exit_code)
{
	_exit(exit_code);
}
