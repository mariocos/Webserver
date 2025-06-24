#include "includes/Routes.hpp"

Routes::Routes() : WebSocket(), _maxConnections(-1), _maxBodySize(-1), _default(false), _isCgi(false), _directoryListening(false)
{
	for (int i = 0; i < 3; i++)
		this->_methods[i] = false;
}

Routes::Routes(int socket, int maxConnections, int maxBodySize, bool flag, std::string root, std::string uri) : WebSocket(socket), _root(root), _uri(uri), _maxConnections(maxConnections), _maxBodySize(maxBodySize), _default(flag), _isCgi(false), _directoryListening(false)
{
	for (int i = 0; i < 3; i++)
		this->_methods[i] = false;
}

Routes::Routes(const Routes &copy) : WebSocket(copy)
{
	*this = copy;
}

Routes	&Routes::operator=(const Routes &copy)
{
	if (this != &copy)
	{
		this->_root = copy._root;
		this->_uri = copy._uri;
		this->_maxConnections = copy._maxConnections;
		this->_maxBodySize = copy._maxBodySize;
		for (size_t i = 0; i < 3; i++)
			this->_methods[i] = copy._methods[i];
	}
	return (*this);
}

Routes::~Routes()
{
}

std::string	&Routes::getRoot()
{
	return (this->_root);
}

std::string	&Routes::getURI()
{
	return (this->_uri);
}

int	Routes::getMaxConnections()
{
	return (this->_maxConnections);
}

int	Routes::getMaxBodySize()
{
	return (this->_maxBodySize);
}

bool	Routes::canDoMethod(int method)
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

bool	Routes::isDefault()
{
	return (this->_default);
}

bool	Routes::isCgi()
{
	return (this->_isCgi);
}

bool	Routes::isListening()
{
	return (this->_directoryListening);
}

void	Routes::setRoot(std::string &root)
{
	this->_root = root;
}

void	Routes::setURI(std::string &uri)
{
	this->_uri = uri;
}

void	Routes::setMaxConnections(int value)
{
	this->_maxConnections = value;
}

void	Routes::setMaxBodySize(int value)
{
	this->_maxBodySize = value;
}

void	Routes::setMethod(int method, bool flag)
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

void	Routes::setAsCgi()
{
	this->_isCgi = true;
}

void	Routes::setAsListening()
{
	this->_directoryListening = true;
}
