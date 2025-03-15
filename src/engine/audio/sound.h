#pragma once

#include <math/vec3.h>

#include <stdbool.h>
#include <stddef.h>

typedef enum {
	SOUND_TYPE_WAV,
	SOUND_TYPE_MP3
} sound_type_t;

typedef struct {
	sound_type_t type;
	void* data;
	size_t size;
	int channels;
	int sampleRate;
	int bytesPerSecond;
	int bitsPerSample;
} sound_file_t;

bool Snd_LoadSoundFromMemory(sound_file_t* sound, sound_type_t type, const void* data, size_t size);
bool Snd_LoadWavFromMemory(sound_file_t* sound, const void* data, size_t size);
bool Snd_LoadMP3FromMemory(sound_file_t* sound, const void* data, size_t size);
bool Snd_LoadSoundFromFile(sound_file_t* sound, const char* path);
bool Snd_FreeFile(sound_file_t* sound);

typedef unsigned int sound_t;

sound_t Snd_LoadSound(sound_file_t* file);
void Snd_UnloadSound(sound_t sound);

typedef struct {
	sound_t sound;
	vec3_t pos;
	float gain;
	bool relative;
} sound_params_t;

typedef struct {
	unsigned int id;
	sound_params_t params;
} sound_stream_t;

void Snd_CreateStream(sound_stream_t* stream);
void Snd_DestroyStream(sound_stream_t* stream);
void Snd_SetStreamSound(sound_stream_t* stream, sound_t sound);
void Snd_PlayStream(sound_stream_t* stream);
void Snd_PauseStream(sound_stream_t* stream);
void Snd_StopStream(sound_stream_t* stream);
void Snd_UpdateStream(sound_stream_t* stream);
