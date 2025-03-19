#include <engine/audio/sound.h>

#include <util/buffer.h>
#include <util/array.h>
#include <util/endian.h>

#include <engine/log.h>
#include <string.h>

#include <platform/memory.h>

struct wav_chunk_t {
	char name[5];
	uint32_t size;
	void* data;
};

struct wav_format_t {
	uint16_t format;
	uint16_t channels;
	uint32_t sampleRate;
	uint32_t bytesPerSecond;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
};

static const char* WAV_MASTER_RIFF = "RIFF";
static const char* WAV_MASTER_WAVE = "WAVE";

bool read_wave_chunk(struct wav_chunk_t* chunk, buffer_t* buffer)
{
	if (!Buffer_Read(buffer, chunk->name, sizeof(chunk->name) - 1)) {
		return false;
	}

	chunk->name[4] = 0;

	if (!Buffer_Read(buffer, &chunk->size, sizeof(chunk->size))) {
		return false;
	}

	char* ptr = (char*)buffer->data;

	chunk->data = ptr + buffer->pos;

	buffer->pos += chunk->size;

	return buffer->pos <= buffer->size;
}

bool Snd_LoadWavFromMemory(sound_file_t* file, const void* data, size_t size)
{
	buffer_t buffer = Buffer_FromMemory((void*)data, size);

	uint32_t riff;

	Buffer_Read(&buffer, &riff, sizeof(riff));

	riff = Endian_HostToLittle32(riff);

	if (riff != Endian_HostToLittle32(*(uint32_t*)WAV_MASTER_RIFF)) {
		return false;
	}

	Buffer_ReadUInt32(&buffer);

	uint32_t wave;

	Buffer_Read(&buffer, &wave, sizeof(wave));

	wave = Endian_HostToLittle32(wave);

	if (wave != Endian_HostToLittle32(wave)) {
		return false;
	}

	struct wav_chunk_t* chunks = Array_Create(struct wav_chunk_t);

	struct wav_chunk_t chnk;

	while (read_wave_chunk(&chnk, &buffer)) {
		Array_Push(chunks, chnk);
	}

	for (size_t i = 0; i < Array_Count(chunks); i++) {
		struct wav_chunk_t c = chunks[i];

		if (strcmp(c.name, "fmt ") == 0) {
			struct wav_format_t* format = c.data;

			file->channels = format->channels;
			file->sampleRate = format->sampleRate;
			file->bytesPerSecond = format->bytesPerSecond;
			file->bitsPerSample = format->bitsPerSample;
		} else if (strcmp(c.name, "data") == 0) {
			file->data = Sys_Malloc(c.size);
			file->size = c.size;

			Sys_MemCpy(c.data, file->data, c.size);
		}
	}

	Array_Destroy(chunks);

	file->type = SOUND_TYPE_WAV;

	return true;
}
