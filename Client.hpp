#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"

class Client
{
private:
	int	_clientSocket;
public:
	Client();
	Client(int client_socket);
	~Client();
	void	setClientSocket(int client_socket);
	int	getClientSocket();
};

Client	*new_connection(int server_socket, int epoll_fd);

#endif