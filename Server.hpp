#ifndef	SERVER_HPP
# define SERVER_HPP

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

# include "webserv.hpp"

class Server
{
private:
	int	_serverSocket;
	int	_epoll_fd;
	int	_epoll_count;
	epoll_event	_event;
	epoll_event	_events[60];
public:
	Server();
	Server(int port, int backlog);
	~Server();
	void	setEpollCount(int count);
	int	getServerSocket();
	int	getEpollFd();
	int	getEpollCount();
	int	handle_connections(std::vector<Client*> &clientList, std::vector<int> &errorFds);
	epoll_event	getEpollEvent();
	epoll_event	*getEpollEventArray();
	epoll_event	getEpollIndex(int index);
	void	addNewSocket(int fd);
	void	addToEpollIndex(int fd, int event_number);
	void	removeFromEpoll(int fd, int event_number);
};

#endif
