#ifndef	SERVER_HPP
# define SERVER_HPP

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

# include "webserv.hpp"

class ServerBlock;

class Server
{
private:
	std::vector<ServerBlock*>	_serverBlocks;
	int	_epoll_fd;
	int	_epoll_count;
	int	_maxEvents;
	epoll_event	*_events;
public:
	Server();
	Server(std::vector<int> ports, std::vector<std::string> names, int backlog);
	~Server();
	void	setEpollCount(int count);
	int	getEpollFd();
	int	getEpollCount();
	int	getMaxEvents();
	int	getServerSocketTriggered(int fd);
	std::vector<ServerBlock*>::iterator getServerBlockTriggered(int fd);
	std::vector<ServerBlock*>	getServerBlocks();
	ServerBlock	getServerBlock(int index);
	epoll_event	*getEpollEventArray();
	epoll_event	getEpollIndex(int index);
	void	handle_connections(std::vector<Client*> &clientList, std::vector<int> &errorFds);
	void	addNewSocket(int fd);
	void	removeFromEpoll(int fd);
	class	SocketCreationException : public std::runtime_error
	{
		public:
			SocketCreationException();
	};
	class	SocketBindException : public std::runtime_error
	{
		public:
			SocketBindException();
	};
	class	EpollCreationException : public std::runtime_error
	{
		public:
			EpollCreationException();
	};
	class	EpollCtlException : public std::runtime_error
	{
		public:
			EpollCtlException();
	};
	class	NoFileToReadException : public std::runtime_error
	{
		public:
			NoFileToReadException();
	};
};

#endif
