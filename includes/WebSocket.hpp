#ifndef	WEBSOCKET_HPP
# define WEBSOCKET_HPP

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

# include <iostream>
# include <sys/epoll.h>
# include <unistd.h>

enum socket_state {
	INACTIVE,
	READING,
	WRITING,
	EDGE,
};

class WebSocket
{
protected:
	int	_socket;
	int	_state;
public:
	WebSocket();
	WebSocket(int socket);
	WebSocket(const WebSocket &copy);
	WebSocket	&operator=(const WebSocket &copy);
	~WebSocket();
	int	getSocketFd();
	int	getSocketState();
	void	setSocketFd(int fd);
	void	setSocketState(int state);
	void	setSocketToReading(int epollFd);
	void	setSocketToWriting(int epollFd);
	void	setSocketToEdge(int epollFd);
	void	addSocketToEpoll(int epollFd);
	void	removeSocketFromEpoll(int epollFd);
};

#endif