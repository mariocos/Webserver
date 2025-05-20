#include "includes/ServerBlock.hpp"

ServerBlock::ServerBlock() : WebSocket(), _maxConnections(-1), _port(-1), _default(false)
{
	for (int i = 0; i < 3; i++)
		_methods[i] = false;
}

ServerBlock::ServerBlock(int port, int backlog, std::string name, int socket, bool flag) : WebSocket(socket), _name(name), _maxConnections(backlog), _port(port), _default(flag)
{
	if (setNonBlocking(this->getSocketFd()) == -1)
		throw NonBlockingException(this->getSocketFd());
	for (int i = 0; i < 3; i++)
		_methods[i] = false;
}

ServerBlock::ServerBlock(const ServerBlock &copy) : WebSocket(copy)
{
	*this = copy;
}

ServerBlock	&ServerBlock::operator=(const ServerBlock &copy)
{
	if (this != &copy)
	{
		this->_name = copy._name;
		this->_maxConnections = copy._maxConnections;
		this->_port = copy._port;
		for (size_t i = 0; i < 3; i++)
			this->_methods[i] = copy._methods[i];
		this->_default = copy._default;
	}
	return (*this);
}

ServerBlock::~ServerBlock()
{
}

std::string	ServerBlock::getBlockName()
{
	return (this->_name);
}

int	ServerBlock::getBlockMaxConnections()
{
	return (this->_maxConnections);
}

int	ServerBlock::getBlockPort()
{
	return (this->_port);
}

bool	ServerBlock::canDoMethod(int method)
{
	switch (method)
	{
		case 0:
			return (this->_methods[GET]);
		case 1:
			return (this->_methods[POST]);
		case 2:
			return (this->_methods[DELETE]);
		default:
			return (false);
	}
}

bool	ServerBlock::isDefault()
{
	return (this->_default);
}

void	ServerBlock::setBlockName(std::string name)
{
	this->_name = name;
}

void	ServerBlock::setBlockMaxConnections(int value)
{
	this->_maxConnections = value;
}

void	ServerBlock::setBlockPort(int port)
{
	this->_port = port;
}

void	ServerBlock::setBlockMethod(int method, bool flag)
{
	switch (method)
	{
		case 0:
			this->_methods[GET] = flag;
			break ;
		case 1:
			this->_methods[POST] = flag;
			break ;
		case 2:
			this->_methods[DELETE] = flag;
			break ;
	}
}
