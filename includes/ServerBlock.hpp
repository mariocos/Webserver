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
	std::map<int, std::string>	_errorPages;
	std::vector<Routes*>	_routes;
	std::string	_name;
	int			_maxConnections;
	int			_connections;
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
	int			getBlockActualConnections();
	int			getBlockPort();
	bool		canDoMethod(int method);
	bool		isDefault();
	bool		isCgi();\
	std::map<int, std::string>	getErrorMap();
	std::map<int, std::string>::iterator	getErrorPage(int key);
	std::vector<Routes*>	&getRoutesVector();
	std::vector<Routes*>::iterator	getDefaultRoute();
	Routes		*getRoute(int index);
	void		setBlockName(std::string name);
	void		setBlockMaxConnections(int value);
	void		increaseConnections();
	void		decreaseConnections();
	void		setErrorPage(int key, std::string path);
	void		setBlockPort(int port);
	void		setBlockMethod(int method, bool flag);
	void		setBlockAsCgi();
	void		setBlockRoutes(std::vector<Routes*> &routes);
};

#endif