#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"

class RequestParse;
class Response;

class Client
{
private:
	int				_clientSocket;
	RequestParse	*_request;
	Response		*_response;
	bool			_pending;
	bool			_keepAlive;
	int				_openFd;
public:
	Client();
	Client(int client_socket);
	~Client();
	void	setClientSocket(int client_socket);
	void	setClientPending(bool pending);
	void	setClientConnection(bool connection);
	void	setClientRequest(RequestParse *request);
	void	setClientResponse(Response *response);
	void	setClientOpenFd(int fd);
	int		getClientSocket();
	bool	getClientPending();
	bool	getClientConnection();
	int	getClientOpenFd();
	RequestParse	*getClientRequest();
	Response		*getClientResponse();
};

Client	*new_connection(int server_socket, int epoll_fd);

#endif