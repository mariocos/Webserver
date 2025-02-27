#include "Client.hpp"

Client::Client() : _clientSocket(0)
{
}

Client::Client(int client_socket) : _clientSocket(client_socket)
{
	check(client_socket);
}

Client::~Client()
{
}

void	Client::setClientSocket(int client_socket)
{
	_clientSocket = client_socket;
	check(_clientSocket);
}

int	Client::getClientSocket()
{
	return (this->_clientSocket);
}
