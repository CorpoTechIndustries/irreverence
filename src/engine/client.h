#pragma once

#include <stdint.h>
#include <engine/net.h>
#include <util/buffer.h>
#include <enet.h>
#include <public/edict.h>

typedef enum {
	CLIENT_CONNECTION_UNINITIALIZED,
	CLIENT_CONNECTION_DISCONNECTED,
	CLIENT_CONNECTION_CONNECTING,
	CLIENT_CONNECTION_CONNECTED,
} client_connection_t;

typedef enum {
	CLIENT_EDICT_FREE,
	CLIENT_EDICT_DORMANT,
	CLIENT_EDICT_NOSIGHT,
	CLIENT_EDICT_SIGHT
} client_edict_state_t;

typedef struct {
	bool running;
	float cur_time;
	client_connection_t state;
	ENetHost* host;
	ENetPeer* peer;
	buffer_t reliable;
	buffer_t unreliable;
	uint32_t challenge;
} client_state_t;

extern client_state_t cl;

void CL_Init(client_state_t* state);
void CL_Close(client_state_t* state);
void CL_Update(client_state_t* state);
void CL_Render(client_state_t* state);
void CL_Connect(client_state_t* state, const char* address, uint16_t port);
void CL_Disconnect(client_state_t* state);
bool CL_NetSend(
	client_state_t* state,
	uint16_t msg, const void* data, size_t size,
	bool reliable
);
