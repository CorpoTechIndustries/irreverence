#include <engine/server.h>
#include <engine/cvar.h>
#include <engine/net.h>
#include <engine/net_messages.h>
#include <engine/engine.h>
#include <engine/log.h>

#include <platform/memory.h>

#include <math/random.h>

server_state_t sv;


MAKE_CVAR_INT(sv_maxplayers, 16);
MAKE_CVAR_STRING(sv_map, "");

static char sv_buffer_data[MAX_NET_PACKET_SIZE];

void SV_Init(server_state_t* state)
{
	CVar_Register(&sv_maxplayers);
	CVar_Register(&sv_map);
	Sys_MemZero(state, sizeof(*state));

	state->buffer = Buffer_FromMemory(sv_buffer_data, sizeof(sv_buffer_data));
}

void SV_Close(server_state_t* state)
{
	if (state->running) {
		SV_Shutdown(state);
	}
}

static bool handle_connection(server_state_t* state, ENetPeer* peer);
static bool handle_disconnect(server_state_t* state, ENetPeer* peer);
static bool handle_receive(server_state_t* state, ENetPeer* peer, ENetPacket* packet);

void SV_Update(server_state_t* state)
{
	if (!state->running) {
		return;
	}

	state->cur_time = Engine_CurTime();

	// No host to actually do networking.
	if (!state->host) {
		return;
	}

	ENetEvent ev;

	while (enet_host_service(state->host, &ev, 0) > 0) {
		switch (ev.type) {
			case ENET_EVENT_TYPE_CONNECT: {
				if (!handle_connection(state, ev.peer)) {
					enet_peer_disconnect_now(ev.peer, 0);
				}

				break;
			}

			case ENET_EVENT_TYPE_DISCONNECT: {
				if (!handle_disconnect(state, ev.peer)) {
					enet_peer_disconnect_now(ev.peer, 0);
				}

				break;
			}

			case ENET_EVENT_TYPE_RECEIVE: {
				handle_receive(state, ev.peer, ev.packet);

				enet_packet_destroy(ev.packet);

				break;
			}
		}
	}

	for (size_t i = 0; i < state->client_count; i++) {
		client_t* client = state->clients + i;

		if (client->state < CLIENT_STATE_CONNECTED) {
			continue;
		}

		if (client->reliable.pos > 0) {
			ENetPacket* reliablePacket = enet_packet_create(
				client->reliable.data, client->reliable.pos, ENET_PACKET_FLAG_RELIABLE
			);

			enet_peer_send(client->peer, 0, reliablePacket);

			client->reliable.pos = 0;
		}

		if (client->unreliable.pos > 0) {
			ENetPacket* unreliablePacket = enet_packet_create(
				client->unreliable.data, client->unreliable.pos, 0
			);

			enet_peer_send(client->peer, 1, unreliablePacket);

			client->unreliable.pos = 0;
		}
	}

	for (size_t i = 0; i < EDICT_MAX_COUNT; i++) {
		if (g_pEdicts[i].free) {
			continue;
		}

		g_GameExports.pThink(&g_pEdicts[i]);

		buffer_t buffer = Buffer_FromMemory(sv_buffer_data, sizeof(sv_buffer_data));

		Buffer_WriteUInt32(&buffer, g_pEdicts[i].id);
		Buffer_Write(&buffer, &g_pEdicts[i].v, sizeof(g_pEdicts[i].v) - sizeof(edict_t*));

		SV_NetBroadcast(state, SVC_EDICT_UPDATE, buffer.data, buffer.pos, false);
	}
}

static bool handle_connection(server_state_t* state, ENetPeer* peer)
{
	// Find free client.

	client_t* client = NULL;

	for (size_t i = 0; i < state->client_count; i++) {
		if (state->clients[i].state != CLIENT_STATE_FREE) {
			continue;
		}

		client = state->clients + i;
	}

	if (!client) {
		return false;
	}

	client->peer = peer;
	client->state = CLIENT_STATE_CONNECTED;

	// TODO: Pass to the game DLL to setup the player.

	peer->data = client;

	// Send the client all the data for the edicts

	for (size_t i = 0; i < EDICT_MAX_COUNT; i++) {
		edict_t* ed = ED_Get(i);

		if (!ed) {
			continue;
		}

		buffer_t buffer = Buffer_FromMemory(sv_buffer_data, sizeof(sv_buffer_data));

		static char className[512];

		g_GameExports.pGetEntityClass(ed, className, sizeof(className));

		Buffer_WriteUInt32(&buffer, i);
		Buffer_WriteString(&buffer, className, strlen(className) + 1);

		SV_NetSend(state, client, SVC_EDICT_CREATE, buffer.data, buffer.pos, true);
	}

	return true;
}

static bool handle_disconnect(server_state_t* state, ENetPeer* peer)
{
	client_t* client = (client_t*)peer->data;

	// Idk what the fuck happened here
	if (!client) {
		return false;
	}

	SV_Disconnect(state, client);

	return true;
}

static bool handle_receive(server_state_t* state, ENetPeer* peer, ENetPacket* packet)
{
	buffer_t buffer = Buffer_FromMemory(packet->data, packet->dataLength);
	client_t* client = (client_t*)peer->data;

	while (buffer.pos < buffer.size) {
		uint16_t msg = Buffer_ReadUInt16(&buffer);

		switch (msg) {
			case CLC_NOP: {
				LOG_INFO("A client sent a NOP to the server");

				break;
			}

			case CLC_DISCONNECT: {
				SV_Disconnect(state, client);

				return true;
			}

		}

	}

	return true;
}

void SV_Host(server_state_t* state)
{
	if (state->running) {
		SV_Shutdown(state);
	}

	state->client_count = sv_maxplayers.value.as_int;
	state->clients = Sys_Calloc(sizeof(*state->clients) * state->client_count);

	for (size_t i = 0; i < state->client_count; i++) {
		state->clients[i].state = CLIENT_STATE_FREE;

		Buffer_Create(&state->clients[i].reliable, MAX_NET_PACKET_SIZE);
		Buffer_Create(&state->clients[i].unreliable, MAX_NET_PACKET_SIZE);
	}

	ENetAddress address;

	address.host = ENET_HOST_ANY;
	address.port = CVar_GetInt("sv_port");

	state->host = enet_host_create(&address, state->client_count, 2, 0, 0);

	if (!state->host) {
		SV_Shutdown(state);
		return;
	}

	state->running = true;
}

void SV_Shutdown(server_state_t* state)
{
	if (!state->running) {
		return;
	}

	if (!state->host) {
		return;
	}

	for (size_t i = 0; i < state->client_count; i++) {
		SV_Disconnect(state, state->clients + i);

		Buffer_Free(&state->clients[i].reliable);
		Buffer_Free(&state->clients[i].unreliable);
	}

	Sys_Free(state->clients);

	enet_host_destroy(state->host);
	state->host = NULL;
	state->running = false;
}

void SV_Disconnect(server_state_t* state, client_t* client)
{
	if (!state->running) {
		return;
	}

	if (client->edict) {
		ED_Free(client->edict->id);
	}

	if (client->peer) {
		enet_peer_disconnect_now(client->peer, 0);
		client->peer = NULL;
	}

	client->state = CLIENT_STATE_FREE;
}

bool SV_NetSend(
	server_state_t* state,
	client_t* client, uint16_t msg, const void* data, size_t size,
	bool reliable
)
{
	buffer_t* buffer = reliable ? &client->reliable : &client->unreliable;

	if (buffer->pos + size + sizeof(msg) >= buffer->size) {
		return false;
	}

	Buffer_WriteUInt16(buffer, msg);
	Buffer_Write(buffer, data, size);

	return true;
}

void SV_NetBroadcast(
	server_state_t* state,
	uint16_t msg, const void* data, size_t size,
	bool reliable
)
{
	for (size_t i = 0; i < state->client_count; i++) {
		SV_NetSend(state, state->clients + i, msg, data, size, reliable);
	}
}
