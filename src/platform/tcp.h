#pragma once

#include <stdint.h>
#include <stddef.h>

typedef int tcp_socket_t;

typedef struct {
	uint32_t address;
	uint16_t port;
} tcp_address_t;

tcp_address_t Sys_TCPLocalAddress(uint16_t port);
tcp_address_t Sys_TCPAddress(const char* address, uint16_t port);

tcp_socket_t Sys_TCPCreate();
void Sys_TCPDestroy(tcp_socket_t socket);

int Sys_TCPBind(tcp_socket_t socket, tcp_address_t address);
int Sys_TCPListen(tcp_socket_t socket, int backlog);
tcp_socket_t Sys_TCPAccept(tcp_socket_t socket, tcp_address_t* address);
int Sys_TCPConnect(tcp_socket_t socket, tcp_address_t address);
int Sys_TCPRead(tcp_socket_t socket, void* buffer, size_t size);
int Sys_TCPWrite(tcp_socket_t socket, const void* data, size_t size);
