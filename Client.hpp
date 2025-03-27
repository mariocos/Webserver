#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "webserv.hpp"

class RequestParse;
class Response;
class Server;

class Client
{
private:
	int				_clientSocket;
	RequestParse	*_request;
	Response		*_response;
	bool			_pending;
	bool			_keepAlive;
	int				_openFd;
	bool			_finishedReading;
	bool			_finishedWriting;
	time_t			_startTime;
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
	void	setClientReadingFlag(bool flag);
	void	setClientWritingFlag(bool flag);
	void	setStartingTime();
	int		getClientSocket();
	bool	getClientPending();
	bool	getClientConnection();
	bool	getClientReadingFlag();
	bool	getClientWritingFlag();
	bool	connectionExpired(int timeoutSec);
	int	getClientOpenFd();
	RequestParse	*getClientRequest();
	Response		*getClientResponse();
	void	readRequest(int client_socket);
	void	handle_connect(int client_socket);
};

void	new_connection(std::vector<Client*> &clientList, std::vector<int> &errorFds, Server &server);

#endif