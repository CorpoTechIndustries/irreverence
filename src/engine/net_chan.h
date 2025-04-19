#pragma once

#include <engine/net.h>

typedef struct {
	net_source_t source;
	net_address_t address;
	size_t last_receive;
	size_t last_send;
	size_t incoming_sequence;
	size_t outgoing_sequence;
	uint16_t port;
	net_buffer_t net_buffer;
} netchan_t;

bool NetChan_Init(netchan_t* channel, net_source_t source, net_address_t address);
void NetChan_Transmit(netchan_t* channel);
void NetChan_Free(netchan_t* channel);
bool NetChan_Process(netchan_t* channel, net_buffer_t* buffer);
