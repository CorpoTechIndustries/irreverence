#include <engine/client.h>
#include <engine/cvar.h>
#include <engine/engine.h>
#include <engine/log.h>
#include <engine/net_messages.h>
#include <public/cl_dll.h>
#include <engine/edict.h>

MAKE_CVAR_STRING(cl_name, "Player");

client_state_t cl;

static char cl_buffer_data[MAX_NET_PACKET_SIZE];
static char cl_reliable_data[MAX_NET_PACKET_SIZE];

static client_entity_t cl_edicts[EDICT_MAX_COUNT];

void CL_Init(client_state_t* state)
{
	CVar_Register(&cl_name);

	state->unreliable = Buffer_FromMemory(cl_buffer_data, sizeof(cl_buffer_data));
	state->reliable = Buffer_FromMemory(cl_reliable_data, sizeof(cl_reliable_data));
	state->running = true;
	state->host = enet_host_create(NULL, 1, 2, 0, 0);

	for (size_t i = 0; i < EDICT_MAX_COUNT; i++) {
		cl_edicts[i].free = true;
	}
}

void CL_Close(client_state_t* state)
{
	if (state->running) {
		CL_Disconnect(state);
	}
}

static void handle_message(client_state_t* state, ENetPacket* packet);

void CL_Update(client_state_t* state)
{
	if (!state->running) {
		return;
	}

	state->cur_time = Engine_CurTime();

	ENetEvent ev;

	while (enet_host_service(state->host, &ev, 0) > 0) {
		switch (ev.type) {
			case ENET_EVENT_TYPE_CONNECT: {
				// We are fully connected to the server now!

				state->state = CLIENT_CONNECTION_CONNECTED;
				state->peer = ev.peer;

				LOG_INFO("We connected to the server!");

				break;
			}

			case ENET_EVENT_TYPE_DISCONNECT: {
				LOG_INFO("We disconnected from the server!");

				state->peer = NULL;
				state->state = CLIENT_CONNECTION_DISCONNECTED;

				break;
			}

			case ENET_EVENT_TYPE_RECEIVE: {

				handle_message(state, ev.packet);

				enet_packet_destroy(ev.packet);

				break;
			}
		}
	}

	if (!state->peer) {
		return;
	}

	if (state->reliable.pos > 0) {
		ENetPacket* reliablePacket = enet_packet_create(
			state->reliable.data, state->reliable.pos, ENET_PACKET_FLAG_RELIABLE
		);

		enet_peer_send(state->peer, 0, reliablePacket);

		state->reliable.pos = 0;
	}

	if (state->unreliable.pos > 0) {
		ENetPacket* unreliablePacket = enet_packet_create(
			state->unreliable.data, state->unreliable.pos, 0
		);

		enet_peer_send(state->peer, 1, unreliablePacket);

		state->unreliable.pos = 0;
	}

	// Nothing to do if we're not fully connected...
	if (state->state != CLIENT_CONNECTION_CONNECTED) {
		return;
	}

	for (size_t i = 0; i < EDICT_MAX_COUNT; i++) {
		if (cl_edicts[i].free) {
			continue;
		}

		g_ClientExports.pEntityThink(&cl_edicts[i]);
	}
}

static void handle_message(client_state_t* state, ENetPacket* packet)
{
	buffer_t buffer = NetBuffer_FromMemory(packet->data, packet->dataLength);

	while (buffer.pos < buffer.size) {
		uint16_t msg = Buffer_ReadUInt16(&buffer);

		switch (msg) {
			case SVC_NOP: {
				LOG_INFO("Server sent a NOP");

				break;
			}

			case SVC_DISCONNECT: {
				LOG_INFO("Server disconnected us?");

				break;
			}

			case SVC_EDICT_CREATE: {
				uint32_t id = Buffer_ReadUInt32(&buffer);

				char class[512];

				Buffer_ReadString(&buffer, class, 512);

				if (!g_ClientExports.pEntityCreate(cl_edicts + id, class)) {
					LOG_ERROR("WE FUCKING FUCKED UP");
					break;
				}

				cl_edicts[id].free = false;

				break;
			}

			case SVC_EDICT_DESTROY: {
				uint32_t id = Buffer_ReadUInt32(&buffer);

				if (cl_edicts[id].free) {
					break;
				}

				g_ClientExports.pEntityDestroy(cl_edicts + id);

				cl_edicts[id].free = true;

				break;
			}

			case SVC_EDICT_UPDATE: {
				uint32_t id = Buffer_ReadUInt32(&buffer);


				// Idk how this happens
				if (cl_edicts[id].free) {
					return;
				}

				Buffer_Read(&buffer, &cl_edicts[id].v, sizeof(cl_edicts[id].v) - sizeof(client_entity_t*));

				break;
			}
		}
	}

}

void CL_Render(client_state_t* state)
{

	if (state->state != CLIENT_CONNECTION_CONNECTED) {
		return;
	}

	for (size_t i = 0; i < EDICT_MAX_COUNT; i++) {
		if (cl_edicts[i].free) {
			continue;
		}

		g_ClientExports.pEntityRender(&cl_edicts[i]);
	}

}

void CL_Connect(client_state_t* state, const char* address, uint16_t port)
{
	if (state->peer) {
		CL_Disconnect(state);
	}

	ENetAddress addr;
	addr.port = port;

	enet_address_set_host(&addr, address);

	state->peer = enet_host_connect(state->host, &addr, 2, 0);

	state->state = CLIENT_CONNECTION_CONNECTING;
}

void CL_Disconnect(client_state_t* state)
{
	if (!state->running) {
		return;
	}

	if (!state->peer) {
		return;
	}

	enet_peer_disconnect_now(state->peer, 0);

	for (size_t i = 0; i < EDICT_MAX_COUNT; i++) {
		if (cl_edicts[i].free) {
			continue;
		}

		g_ClientExports.pEntityDestroy(&cl_edicts[i]);

		cl_edicts[i].free = true;
	}
}

bool CL_NetSend(
	client_state_t* state,
	uint16_t msg, const void* data, size_t size,
	bool reliable
)
{
	buffer_t* buffer = reliable ? &state->reliable : &state->unreliable;

	if (buffer->pos + size + sizeof(msg) >= buffer->size) {
		return false;
	}

	Buffer_WriteUInt16(buffer, msg);
	Buffer_Write(buffer, data, size);

	return true;
}
