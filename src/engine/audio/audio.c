#include <engine/audio/audio.h>

#include <stddef.h>
#include <AL/alc.h>

static ALCcontext* s_pALContext = NULL;
static ALCdevice* s_pALDevice = NULL;


bool Snd_Init()
{
	s_pALDevice = alcOpenDevice(NULL);

	if (!s_pALDevice) {
		return false;
	}

	s_pALContext = alcCreateContext(s_pALDevice, NULL);

	if (!s_pALContext) {
		return false;
	}

	alcMakeContextCurrent(s_pALContext);

	return true;
}

void Snd_Destroy()
{
	if (s_pALContext) {
		alcDestroyContext(s_pALContext);
	}

	if (s_pALDevice) {
		alcCloseDevice(s_pALDevice);
	}
}
