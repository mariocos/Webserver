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

class ServerBlock : public WebSocket
{
private:
	std::string	_name;
	int			_maxConnections;
	int			_port;
	bool		_methods[3];
	bool		_default;
public:
	ServerBlock();
	ServerBlock(int port, int backlog, std::string name, int socket, bool flag);
	ServerBlock(const ServerBlock &copy);
	ServerBlock	&operator=(const ServerBlock &copy);
	~ServerBlock();
	std::string	getBlockName();
	int			getBlockMaxConnections();
	int			getBlockPort();
	bool		canDoMethod(int method);
	bool		isDefault();
	void		setBlockName(std::string name);
	void		setBlockMaxConnections(int value);
	void		setBlockPort(int port);
	void		setBlockMethod(int method, bool flag);
};

#endif