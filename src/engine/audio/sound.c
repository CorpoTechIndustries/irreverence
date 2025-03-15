#include <engine/audio/sound.h>
#include <platform/path.h>
#include <platform/memory.h>
#include <string.h>

#include <AL/al.h>

bool Snd_LoadSoundFromMemory(sound_file_t* file, sound_type_t type, const void* data, size_t size)
{
	switch (type) {
		case SOUND_TYPE_WAV: {
			return Snd_LoadWavFromMemory(file, data, size);
		}

		case SOUND_TYPE_MP3: {
			return Snd_LoadMP3FromMemory(file, data, size);
		}
	}

	return false;
}

static bool path_has_extension(const char* path, const char* ext)
{
	const char* path_ext = Sys_PathGetExtension(path);

	return strcmp(path_ext, ext) == 0;
}

typedef bool(*load_fn_t)(sound_file_t* file, const void* data, size_t size);

struct {
	const char* ext;
	load_fn_t fn;
} load_fns[] = {
	{ "mp3", Snd_LoadMP3FromMemory },
	{ "wav", Snd_LoadWavFromMemory },
	{ "wave", Snd_LoadWavFromMemory }
};

bool Snd_LoadSoundFromFile(sound_file_t* file, const char* path)
{
	for (size_t i = 0; i < sizeof(load_fns) / sizeof(load_fns[0]); i++) {
		if (!path_has_extension(path, load_fns[i].ext)) {
			continue;
		}

		size_t size = 0;
		void* data = Sys_PathReadFile(path, &size);

		bool ret = load_fns[i].fn(file, data, size);

		Sys_Free(data);

		return ret;
	}

	return false;
}

bool Snd_FreeFile(sound_file_t* file)
{
	if (file->data) {
		Sys_Free(file->data);
	}

	return true;
}

sound_t Snd_LoadSound(sound_file_t* file)
{
	if (!file->data) {
		return 0;
	}

	ALenum format = 0;

	if (file->channels == 2) {
		if (file->bitsPerSample == 16) {
			format = AL_FORMAT_STEREO16;
		} else if (file->bitsPerSample == 8) {
			format = AL_FORMAT_STEREO8;
		}
	} else if (file->channels == 1) {
		if (file->bitsPerSample == 16) {
			format = AL_FORMAT_MONO16;
		} else if (file->bitsPerSample == 8) {
			format = AL_FORMAT_MONO8;
		}
	}

	if (!format) {
		return 0;
	}

	sound_t snd;

	alGenBuffers(1, &snd);

	alBufferData(snd, format, file->data, file->size, file->sampleRate);

	return snd;
}

void Snd_UnloadSound(sound_t sound)
{
	alDeleteBuffers(1, &sound);
}

void Snd_CreateStream(sound_stream_t* stream)
{
	alGenSources(1, &stream->id);
}

void Snd_DestroyStream(sound_stream_t* stream)
{
	alDeleteSources(1, &stream->id);
}

void Snd_SetStreamSound(sound_stream_t* stream, sound_t sound)
{
	alSourcei(stream->id, AL_BUFFER, sound);
	stream->params.sound = sound;
}

void Snd_PlayStream(sound_stream_t* stream)
{
	alSourcePlay(stream->id);
}

void Snd_PauseStream(sound_stream_t* stream)
{
	alSourcePause(stream->id);
}

void Snd_StopStream(sound_stream_t* stream)
{
	alSourceStop(stream->id);
}

void Snd_UpdateStream(sound_stream_t* stream)
{
	unsigned int id = stream->id;
	sound_params_t* params = &stream->params;
	alSourcefv(id, AL_POSITION, params->pos.v);
	alSourcef(id, AL_GAIN, params->gain);
	alSourcei(id, AL_SOURCE_RELATIVE, params->relative);
}
