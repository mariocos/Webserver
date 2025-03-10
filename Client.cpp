#include "Client.hpp"

Client::Client() : _clientSocket(0), _request(NULL), _response(NULL), _pending(false), _keepAlive(false), _openFd(-1)
{
}

Client::Client(int client_socket) : _clientSocket(client_socket), _request(NULL), _response(NULL), _pending(false), _keepAlive(false),_openFd(-1)
{
	std::cout<<GREEN<<"Client constructor called"<<RESET<<std::endl;
	check(client_socket);
	_response = new Response;
	_request = new RequestParse;
	std::string *buffer = new std::string;
	_response->setBuffer(buffer);
	_request->setBuffer(buffer);
}

Client::~Client()
{
	delete _response->getBuffer();
	delete _request;
	delete _response;
	std::cout<<RED<<"Client Destructor"<<RESET<<std::endl;
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

void	Client::setClientConnection(bool connection)
{
	this->_keepAlive = connection;
}

void	Client::setClientRequest(RequestParse *request)
{
	this->_request = request;
}

void	Client::setClientResponse(Response *response)
{
	this->_response = response;
}

void	Client::setClientOpenFd(int fd)
{
	this->_openFd = fd;
}

int	Client::getClientSocket()
{
	return (this->_clientSocket);
}

bool	Client::getClientPending()
{
	return (this->_pending);
}

bool	Client::getClientConnection()
{
	return (this->_keepAlive);
}

int	Client::getClientOpenFd()
{
	return (_openFd);
}

RequestParse	*Client::getClientRequest()
{
	return (this->_request);
}

Response	*Client::getClientResponse()
{
	return (this->_response);
}
