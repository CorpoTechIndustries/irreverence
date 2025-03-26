#include <engine/net.h>
#include <util/endian.h>

#include <engine/log.h>

void NetBuffer_Create(net_buffer_t* buffer, size_t max_size)
{
	Buffer_Create(buffer, max_size);
}

void NetBuffer_CopyMemory(net_buffer_t* buffer, const void* ptr, size_t size)
{
	Buffer_CopyMemory(buffer, ptr, size);
}

void NetBuffer_Free(net_buffer_t* buffer)
{
	Buffer_Free(buffer);
}

uint8_t NetBuffer_ReadUInt8(net_buffer_t* buffer)
{
	return Buffer_ReadUInt8(buffer);
}

uint16_t NetBuffer_ReadUInt16(net_buffer_t* buffer)
{
	uint16_t value = Buffer_ReadUInt16(buffer);

	return Endian_BigToHost16(value);
}

uint32_t NetBuffer_ReadUInt32(net_buffer_t* buffer)
{
	uint32_t value = Buffer_ReadUInt32(buffer);

	return Endian_BigToHost32(value);
}

float NetBuffer_ReadFloat(net_buffer_t* buffer)
{
	uint32_t value = NetBuffer_ReadUInt32(buffer);

	return *(float*)&value;
}

void NetBuffer_WriteUInt8(net_buffer_t* buffer, uint8_t value)
{
	Buffer_WriteUInt8(buffer, value);
}

void NetBuffer_WriteUInt16(net_buffer_t* buffer, uint16_t value)
{
	Buffer_WriteUInt16(buffer, Endian_HostToBig16(value));
}

void NetBuffer_WriteUInt32(net_buffer_t* buffer, uint32_t value)
{
	Buffer_WriteUInt32(buffer, Endian_HostToBig32(value));
}

void NetBuffer_WriteFloat(net_buffer_t* buffer, float value)
{
	NetBuffer_WriteUInt32(buffer, *(uint32_t*)&value);
}

static udp_socket_t s_Sockets[NET_COUNT] = {0};

bool Net_Init()
{
	for (size_t i = 0; i < NET_COUNT; i++) {
		s_Sockets[i] = -1;
	}

	return true;
}

void Net_Close()
{
	for (size_t i = 0; i < NET_COUNT; i++) {
		if (s_Sockets[i] < 0) {
			continue;
		}

		Sys_UDPDestroy(s_Sockets[i]);
	}
}

// TODO: Replace any ports in here with the port cvar when cvars are a thing.
bool Net_Setup(bool multiplayer)
{
	if (multiplayer) {
		uint16_t port = 27015;
		for (size_t i = 0; i < NET_COUNT; i++) {
			if (s_Sockets[i] > -1) {
				Sys_UDPDestroy(s_Sockets[i]);
			}

			s_Sockets[i] = Sys_UDPCreate();

			udp_address_t local = Sys_UDPLocalAddress(port++);

			port = 0;

			if (!Sys_UDPBind(s_Sockets[i], local)) {
				LOG_ERROR("Could not bind for net setup");
				Net_Setup(false);
				return false;
			}

			udp_address_t address;

			if (!Sys_UDPGetAddress(s_Sockets[i], &address)) {
				LOG_INFO("Address error?");
			}

			uint16_t port;
			const char* addr = Sys_UDPAddressToString(address, &port);

			LOG_INFO("Bound %s to %s:%u", (i == NET_SERVER ? "SERVER" : "CLIENT"), addr, port);
		}

		return true;
	}


	for (size_t i = 0; NET_COUNT; i++) {
		Sys_UDPDestroy(s_Sockets[i]);
		s_Sockets[i] = -1;
	}


	return true;
}

int Net_SendPacket(net_source_t source, const void* data, size_t size, net_address_t address)
{
	return Sys_UDPSendTo(s_Sockets[source], address, data, size);
}

int Net_ReadPacket(net_source_t source, void* data, size_t size, net_address_t* from)
{
	return Sys_UDPRecvFrom(s_Sockets[source], from, data, size);
}

net_address_t Net_LocalAddress(uint16_t port)
{
	return Sys_UDPLocalAddress(port);
}

net_address_t Net_Address(const char* address, uint16_t port)
{
	return Sys_UDPAddress(address, port);
}

const char* Net_AddressToString(net_address_t address, uint16_t* port)
{
	return Sys_UDPAddressToString(address, port);
}
