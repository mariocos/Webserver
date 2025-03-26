#include "Client.hpp"

Client::Client() : _clientSocket(0), _request(NULL), _response(NULL), _pending(false), _keepAlive(false), _openFd(-1), _finishedReading(false), _finishedWriting(true)
{
}

Client::Client(int client_socket) : _clientSocket(client_socket), _request(NULL), _response(NULL), _pending(false), _keepAlive(false),_openFd(-1), _finishedReading(false), _finishedWriting(true)
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

void	Client::setClientReadingFlag(bool flag)
{
	this->_finishedReading = flag;
}

void	Client::setClientWritingFlag(bool flag)
{
	this->_finishedWriting = flag;
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

bool	Client::getClientReadingFlag()
{
	return (this->_finishedReading);
}

bool	Client::getClientWritingFlag()
{
	return (this->_finishedWriting);
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

void	Client::readRequest(int client_socket)
{
	this->getClientRequest()->readToBuffer(client_socket, this);
	if (this->getClientRequest()->get_buffer().find("Connection: keep-alive") != std::string::npos)
		this->setClientConnection(true);
	this->getClientRequest()->buildRequest(this->getClientRequest()->get_buffer().c_str());
}

void	Client::handle_connect(int client_socket)
{
	try
	{
		this->getClientRequest()->execute_response(client_socket, this);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	if (this->getClientWritingFlag() == true)
	{
		close(client_socket);
		std::cout<<YELLOW<<"Closing connection..."<<RESET<<std::endl;
	}
}
