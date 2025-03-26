#pragma once

#include <stdint.h>
#include <stddef.h>

typedef int udp_socket_t;

typedef struct {
	uint32_t address;
	uint16_t port;
} udp_address_t;

udp_address_t Sys_UDPLocalAddress(uint16_t port);
udp_address_t Sys_UDPAddress(const char* address, uint16_t port);

const char* Sys_UDPAddressToString(udp_address_t address, uint16_t* port);

udp_socket_t Sys_UDPCreate();
void Sys_UDPDestroy(udp_socket_t);

int Sys_UDPBind(udp_socket_t socket, udp_address_t address);
int Sys_UDPConnect(udp_socket_t socket, udp_address_t address);
int Sys_UDPSendTo(udp_socket_t socket, udp_address_t address, const void* data, size_t size);
int Sys_UDPRecvFrom(udp_socket_t socket, udp_address_t* address, void* buffer, size_t size);
int Sys_UDPGetAddress(udp_socket_t socket, udp_address_t* address);
