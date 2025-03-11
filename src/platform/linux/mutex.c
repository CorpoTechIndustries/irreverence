#include <platform/mutex.h>
#include <platform/memory.h>

#define _GNU_SOURCE
#include <pthread.h>

struct mutex_wrapper_t {
	pthread_mutex_t mutex;
};

mutex_t Sys_MutexCreate()
{
	struct mutex_wrapper_t* m = Sys_Calloc(sizeof(*m));

	m->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;

	return m;
}

void Sys_MutexDestroy(mutex_t mutex)
{
	struct mutex_wrapper_t* m = (struct mutex_wrapper_t*)mutex;

	pthread_mutex_destroy(&m->mutex);

	Sys_Free(m);
}

int Sys_MutexLock(mutex_t mutex)
{
	struct mutex_wrapper_t* m = (struct mutex_wrapper_t*)mutex;

	return pthread_mutex_lock(&m->mutex) == 0;
}

int Sys_MutexTryLock(mutex_t mutex, size_t ms)
{
	struct mutex_wrapper_t* m = (struct mutex_wrapper_t*)mutex;
	struct timespec t;

	t.tv_sec = ms / 1000;
	t.tv_nsec = (ms % 1000) * 1000000;

	return pthread_mutex_timedlock(&m->mutex, &t);
}

int Sys_MutexUnlock(mutex_t mutex)
{
	struct mutex_wrapper_t* m = (struct mutex_wrapper_t*)mutex;

	return pthread_mutex_unlock(&m->mutex);
}
