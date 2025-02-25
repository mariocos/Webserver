#include "Client.hpp"

Client::Client() : _clientSocket(0), _clientEpoll(0)
{
}

Client::Client(int client_socket, int client_epoll) : _clientSocket(client_socket), _clientEpoll(client_epoll)
{
	check(client_socket);
	check(client_epoll);
}

Client::~Client()
{
}

void	Client::setClientSocket(int client_socket)
{
	_clientSocket = client_socket;
	check(_clientSocket);
}

void	Client::setClientEpoll(int client_epoll)
{
	_clientEpoll = client_epoll;
	check(_clientEpoll);
}

int	Client::getClientSocket()
{
	return (this->_clientSocket);
}

int	Client::getClientEpoll()
{
	return (this->_clientEpoll);
}
