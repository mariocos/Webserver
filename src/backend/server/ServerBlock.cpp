#include "../../../includes/ServerBlock.hpp"

ServerBlock::ServerBlock() : WebSocket(), _maxConnections(-1), _connections(0), _port(-1), _default(false)
{
	for (int i = 0; i < 3; i++)
		_methods[i] = false;
}

ServerBlock::ServerBlock(int socket, int port, int backlog, std::string domainName, bool flag) : WebSocket(socket), _name(domainName), _maxConnections(backlog), _connections(0), _port(port), _default(flag), _isCgi(false)
{
	if (domainName.empty())
		this->_name = "localhost";
	if (this->_socket != -1)
	{
		if (setNonBlocking(this->_socket) == -1)
			throw NonBlockingException(this->_socket);
	}
	for (int i = 0; i < 3; i++)
		this->_methods[i] = false;
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
	for (std::vector<Routes*>::iterator it = _routes.begin(); it != _routes.end(); ++it)
		delete *it;
	_routes.clear();
	_errorPages.clear();
	printLog("INFO", this, NULL, NULL, 2, "");
}

std::string	ServerBlock::getBlockName()
{
	return (this->_name);
}

int	ServerBlock::getBlockMaxConnections()
{
	return (this->_maxConnections);
}

int	ServerBlock::getBlockActualConnections()
{
	return (this->_connections);
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

bool	ServerBlock::isCgi()
{
	return (this->_isCgi);
}

std::map<int, std::string>	&ServerBlock::getErrorMap()
{
	return (this->_errorPages);
}

std::map<int, std::string>::iterator	ServerBlock::getErrorPage(int key)
{
	return (this->_errorPages.find(key));
}

std::vector<Routes*>	&ServerBlock::getRoutesVector()
{
	return (this->_routes);
}

std::vector<Routes*>::iterator	ServerBlock::getDefaultRoute()
{
	std::vector<Routes*>::iterator	it = this->_routes.begin();
	while (it != this->_routes.end())
	{
		if ((*it)->isDefault())
			return (it);
		it++;
	}
	return (this->_routes.end());
}

Routes	*ServerBlock::getRoute(int index)
{
	return (this->_routes[index]);
}

void	ServerBlock::setBlockName(std::string name)
{
	this->_name = name;
}

void	ServerBlock::setBlockMaxConnections(int value)
{
	this->_maxConnections = value;
}

void	ServerBlock::increaseConnections()
{
	this->_connections += 1;
}

void	ServerBlock::decreaseConnections()
{
	if (this->_connections > 0)
		this->_connections -= 1;
}

void	ServerBlock::setErrorPage(int key, std::string path)
{
	this->_errorPages[key] = path;
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

void	ServerBlock::setBlockAsCgi()
{
	this->_isCgi = true;
}

void	ServerBlock::setBlockRoutes(std::vector<Routes*> &routes)
{
	this->_routes = routes;
}

void	ServerBlock::setAsDefault()
{
	this->_default = true;
}
