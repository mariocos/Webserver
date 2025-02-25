#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"

class Client
{
private:
	int	_clientSocket;
	int	_clientEpoll;
public:
	Client();
	Client(int client_socket, int client_epoll);
	~Client();
	void	setClientSocket(int client_socket);
	void	setClientEpoll(int client_epoll);
	int	getClientSocket();
	int	getClientEpoll();
};

Client	*new_connection(int server_socket, int epoll_fd);

#endif