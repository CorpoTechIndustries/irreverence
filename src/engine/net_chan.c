#include <engine/net_chan.h>

bool NetChan_Init(netchan_t* channel, net_source_t source, net_address_t adrress)
{
	NetBuffer_Create(&channel->net_buffer, MAX_NET_PACKET_SIZE);
	channel->source = source;

	return true;
}

void NetChan_Free(netchan_t* channel)
{
	NetBuffer_Free(&channel->net_buffer);
}

void NetChan_Transmit(netchan_t* channel)
{
	if (channel->net_buffer.pos < 1) {
		return;
	}

	Net_SendPacket(channel->source, channel->net_buffer.data, channel->net_buffer.pos, channel->address);

	channel->net_buffer.pos = 0;
}

bool NetChan_Process(netchan_t* channel, net_buffer_t* buffer)
{

}
