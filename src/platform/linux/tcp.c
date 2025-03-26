#include <platform/tcp.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

tcp_address_t Sys_TCPLocalAddress(uint16_t port)
{
	tcp_address_t addr;

	addr.address = htonl(INADDR_ANY);
	addr.port = htons(port);

	return addr;
}

tcp_address_t Sys_TCPAddress(const char* address, uint16_t port)
{
	tcp_address_t addr;

	addr.address = inet_addr(address);
	addr.port = htons(port);

	return addr;
}

tcp_socket_t Sys_TCPCreate()
{
	return socket(AF_INET, SOCK_STREAM, 0);
}

void Sys_TCPDestroy(tcp_socket_t socket)
{
	close(socket);
}

int Sys_TCPBind(tcp_socket_t socket, tcp_address_t address)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address.address;
	addr.sin_port = address.port;

	return bind(socket, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

int Sys_TCPListen(tcp_socket_t socket, int backlog)
{
	return listen(socket, backlog) == 0;
}

tcp_socket_t Sys_TCPAccept(tcp_socket_t socket, tcp_address_t* address)
{
	struct sockaddr_in addr;
	socklen_t size = sizeof(addr);

	tcp_socket_t client = accept(socket, (struct sockaddr*)&addr, &size);

	if (client < 0) {
		return -1;
	}

	if (size != sizeof(addr)) {
		close(client);
		return -1;
	}

	address->address = addr.sin_addr.s_addr;
	address->port = addr.sin_port;

	return client;
}

int Sys_TCPConnect(tcp_socket_t socket, tcp_address_t address)
{
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address.address;
	addr.sin_port = address.port;

	return connect(socket, (struct sockaddr*)&addr, sizeof(addr)) == 0;
}

int Sys_TCPRead(tcp_socket_t socket, void* buffer, size_t size)
{
	return read(socket, buffer, size);
}

int Sys_TCPWrite(tcp_socket_t socket, const void* data, size_t size)
{
	return write(socket, data, size);
}
