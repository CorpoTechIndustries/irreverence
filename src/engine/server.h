#pragma once

#include <stddef.h>
#include <stdint.h>
#include <enet.h>
#include <engine/net.h>
#include <engine/edict.h>

typedef enum {
	CLIENT_STATE_FREE,
	CLIENT_STATE_ZOMBIE,
	CLIENT_STATE_CONNECTED,
	CLIENT_STATE_SPAWNED
} client_con_state_t;

typedef struct {
	ENetPeer* peer;

	client_con_state_t state;
	edict_t* edict;

	float last_message;
	float last_connect;

	buffer_t reliable;
	buffer_t unreliable;

	// Challenge that is sent back and forth to make sure
	// we're getting a message from the actual client and not a spoofed IP.
	size_t challenge;
} client_t;

typedef struct {
	net_address_t address;
	uint32_t challenge;
	float time;
} challenge_t;

#define MAX_CHALLENGES 512

typedef struct {
	bool running;
	ENetHost* host;
	size_t client_count;
	client_t* clients;
	buffer_t buffer;
	float cur_time;

	challenge_t challenges[MAX_CHALLENGES];
} server_state_t;

extern server_state_t sv;

void SV_Init(server_state_t* state);
void SV_Close(server_state_t* state);
void SV_Update(server_state_t* state);
void SV_Host(server_state_t* state);
void SV_Shutdown(server_state_t* state);
void SV_Disconnect(server_state_t* state, client_t* client);
bool SV_NetSend(
	server_state_t* state,
	client_t* client, uint16_t msg, const void* data, size_t size,
	bool reliable
);

void SV_NetBroadcast(
	server_state_t* state,
	uint16_t msg, const void* data, size_t size,
	bool reliable
);

edict_t* SV_CreateEdict(const char* name);
void SV_DestroyEdict(size_t id);
