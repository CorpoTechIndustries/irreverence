#include <engine/net.h>
#include <util/endian.h>

#include <engine/log.h>
#include <engine/cvar.h>

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

MAKE_CVAR_INT(sv_port, 27015);

bool Net_Init()
{
	for (size_t i = 0; i < NET_COUNT; i++) {
		s_Sockets[i] = -1;
	}

	CVar_Register(&sv_port);

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
		if (s_Sockets[NET_SERVER] > -1) {
			Sys_UDPDestroy(s_Sockets[NET_SERVER]);
			s_Sockets[NET_SERVER] = -1;
		}

		if (s_Sockets[NET_CLIENT] > -1) {
			Sys_UDPDestroy(s_Sockets[NET_SERVER]);
			s_Sockets[NET_CLIENT] = -1;
		}

		// Server
		{
			s_Sockets[NET_SERVER] = Sys_UDPCreate();

			udp_address_t local = Sys_UDPLocalAddress(sv_port.value.as_int);

			if (!Sys_UDPBind(s_Sockets[NET_SERVER], local)) {
				LOG_ERROR("Could not bind server...");
				Net_Setup(false);
				return false;
			}

			LOG_INFO("Bound server on port %u", sv_port.value.as_int);
		}

		// Client
		{
			s_Sockets[NET_CLIENT] = Sys_UDPCreate();

			udp_address_t local = Sys_UDPLocalAddress(0);

			if (!Sys_UDPBind(s_Sockets[NET_CLIENT], local)) {
				LOG_ERROR("Could not bind server...");
				Net_Setup(false);
				return false;
			}

			udp_address_t address;
			Sys_UDPGetAddress(s_Sockets[NET_CLIENT], &address);

			uint16_t port;
			Sys_UDPAddressToString(address, &port);

			LOG_INFO("Bound client on port %u", port);
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
