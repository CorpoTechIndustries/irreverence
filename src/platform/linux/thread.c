#include <platform/thread.h>
#include <platform/memory.h>

#define _GNU_SOURCE
#include <pthread.h>

struct thread_wrapper_t {
	pthread_t id;
	void* data;
	thread_fn_t fn;
};

static thread_t s_MainThread = 0;

void Sys_ThreadInit()
{
	s_MainThread = Sys_ThreadCurrent();
}

static void* call_wrapper(void* data)
{
	struct thread_wrapper_t* thr = (struct thread_wrapper_t*)data;

	if (thr->fn) {
		thr->fn((thread_t)thr->id, thr->data);
	}

	if (thr->data) {
		Sys_Free(thr->data);
	}

	Sys_Free(thr);

	return NULL;
}

thread_t Sys_ThreadCreate(thread_fn_t fn, const void* data, size_t data_size)
{
	struct thread_wrapper_t* thr = (struct thread_wrapper_t*)Sys_Calloc(sizeof(*thr));

	thr->fn = fn;

	if (data) {
		thr->data = Sys_Malloc(data_size);
		Sys_MemCpy(data, thr->data, data_size);
	}

	pthread_t new_thread;

	if (pthread_create(&new_thread, NULL, call_wrapper, thr) != 0) {
		if (thr->data) {
			Sys_Free(thr->data);
		}

		Sys_Free(thr);
		return 0;
	}

	thr->id = new_thread;

	return thr->id;
}

int Sys_ThreadKill(thread_t thread)
{
	return pthread_cancel(thread) == 0;
}

int Sys_ThreadJoin(thread_t thread)
{
	return pthread_join(thread, NULL) == 0;
}

int Sys_ThreadTryJoin(thread_t thread, size_t ms)
{
	struct timespec t;

	t.tv_sec = ms / 1000;
	t.tv_nsec = (ms % 1000) * 1000000;

	return pthread_timedjoin_np(thread, NULL, &t) == 0;
}

thread_t Sys_ThreadCurrent()
{
	return (thread_t)pthread_self();
}

int Sys_ThreadIsMain()
{
	return s_MainThread = Sys_ThreadCurrent();
}
