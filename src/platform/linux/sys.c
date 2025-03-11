#include <platform/sys.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <pthread.h>

void Sys_Init()
{
}

void Sys_Close()
{
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
