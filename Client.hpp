#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"

class RequestParse;

class Client
{
private:
	int	_clientSocket;
	RequestParse	*_request;
	bool	_pending;
	int	_openFd;
public:
	Client();
	Client(int client_socket);
	~Client();
	void	setClientSocket(int client_socket);
	void	setClientPending(bool pending);
	void	setClientRequest(RequestParse *request);
	void	setClientOpenFd(int fd);
	int		getClientSocket();
	int		getClientPending();
	int		getClientOpenFd();
	RequestParse	*getClientRequest();
};

Client	new_connection(int server_socket, int epoll_fd);

#endif