#include <platform/udp.h>
#include <platform/io.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

udp_address_t Sys_UDPLocalAddress(uint16_t port)
{
	udp_address_t address;

	address.address = htonl(INADDR_ANY);
	address.port = htons(port);

	return address;
}

udp_address_t Sys_UDPAddress(const char* address, uint16_t port)
{
	udp_address_t addr;

	addr.address = inet_addr(address);
	addr.port = htons(port);

	return addr;
}

static char str[INET_ADDRSTRLEN];

const char* Sys_UDPAddressToString(udp_address_t address, uint16_t* port)
{
	inet_ntop(AF_INET, &address.address, str, INET_ADDRSTRLEN);

	if (port) {
		*port = ntohs(address.port);
	}

	return str;
}

udp_socket_t Sys_UDPCreate()
{
	udp_socket_t sock = socket(AF_INET, SOCK_DGRAM, 0);

	uint32_t i = 1;

	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &i, sizeof(i));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));

	return sock;
}

void Sys_UDPDestroy(udp_socket_t socket)
{
	close(socket);
}

int Sys_UDPBind(udp_socket_t socket, udp_address_t address)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address.address;
	addr.sin_port = address.port;

	return bind(socket, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

int Sys_UDPConnect(udp_socket_t socket, udp_address_t address)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address.address;
	addr.sin_port = address.port;

	return connect(socket, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

int Sys_UDPSendTo(udp_socket_t socket, udp_address_t address, const void* data, size_t size)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address.address;
	addr.sin_port = address.port;

	return sendto(socket, data, size, 0, (struct sockaddr*)&addr, sizeof(addr));
}

int Sys_UDPRecvFrom(udp_socket_t socket, udp_address_t* address, void* buffer, size_t size)
{
	struct sockaddr_storage addr;
	socklen_t addr_size = sizeof(addr);

	int result = recvfrom(socket, buffer, size, MSG_DONTWAIT, (struct sockaddr*)&addr, &addr_size);

	if (addr_size != sizeof(struct sockaddr_in)) {
		return -1;
	}

	if (address) {
		struct sockaddr_in* in = (struct sockaddr_in*)&addr;
		address->address = in->sin_addr.s_addr;
		address->port = in->sin_port;
	}

	return result;
}

int Sys_UDPGetAddress(udp_socket_t socket, udp_address_t* address)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	int res = getsockname(socket, (struct sockaddr*)&addr, &len);

	address->address = addr.sin_addr.s_addr;
	address->port = addr.sin_port;

	return res == 0;
}
