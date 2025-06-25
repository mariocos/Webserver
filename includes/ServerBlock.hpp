#ifndef	SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

# include "webserv.hpp"

enum methods {
	GET,
	POST,
	DELETE,
};

class Routes;

class ServerBlock : public WebSocket
{
private:
	std::vector<Routes*>	_routes;
	std::string	_name;
	int			_maxConnections;
	int			_port;
	bool		_methods[3];
	bool		_default;
	bool		_isCgi;
public:
	ServerBlock();
	ServerBlock(int socket, int port, int backlog, std::string domainName, bool flag);
	ServerBlock(const ServerBlock &copy);
	ServerBlock	&operator=(const ServerBlock &copy);
	~ServerBlock();
	std::string	getBlockName();
	int			getBlockMaxConnections();
	int			getBlockPort();
	bool		canDoMethod(int method);
	bool		isDefault();
	bool		isCgi();
	std::vector<Routes*>	&getRoutesVector();
	std::vector<Routes*>::iterator	getDefaultRoute();
	Routes		*getRoute(int index);
	void		setBlockName(std::string name);
	void		setBlockMaxConnections(int value);
	void		setBlockPort(int port);
	void		setBlockMethod(int method, bool flag);
	void		setBlockAsCgi();
	void		setBlockRoutes(std::vector<Routes*> &routes);
};

#endif