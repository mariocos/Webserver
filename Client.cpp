#include "Client.hpp"

Client::Client() : _clientSocket(0), _request(NULL), _pending(false), _openFd(-1)
{
}

Client::Client(int client_socket) : _clientSocket(client_socket), _request(NULL), _pending(false), _openFd(-1)
{
	std::cout<<GREEN<<"Client constructor called"<<RESET<<std::endl;
	check(client_socket);
}

Client::~Client()
{
	delete _request;
}

void	Client::setClientSocket(int client_socket)
{
	this->_clientSocket = client_socket;
	check(_clientSocket);
}

void	Client::setClientPending(bool pending)
{
	this->_pending = pending;
}

void	Client::setClientRequest(RequestParse *request)
{
	this->_request = request;
}

void	Client::setClientOpenFd(int fd)
{
	this->_openFd = fd;
}

int	Client::getClientSocket()
{
	return (this->_clientSocket);
}

int	Client::getClientPending()
{
	return (this->_pending);
}

int	Client::getClientOpenFd()
{
	return (this->_openFd);
}

RequestParse	*Client::getClientRequest()
{
	return (this->_request);
}
