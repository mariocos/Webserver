#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../parse_request/RequestParse.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "File.hpp"
# include "WebSocket.hpp"


class RequestParse;
class Response;
class Server;
class File;

class Client : public WebSocket
{
private:
	int				_portTriggered;
	std::string		_domainTriggered;
	RequestParse	*_request;
	Response		*_response;
	File			*_file;
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
	void	setClientPending(bool pending);
	void	setClientConnection(bool connection);
	void	setClientRequest(RequestParse *request);
	void	setClientResponse(Response *response);
	void	setClientOpenFd(int fd);
	void	setClientReadingFlag(bool flag);
	void	setClientWritingFlag(bool flag);
	void	setClientFile(File *file);
	void	setStartingTime();
	void	setPortTriggered(int port);
	void	setDomainTriggered(std::string name);
	bool	getClientPending();
	bool	getClientConnection();
	bool	getClientReadingFlag();
	bool	getClientWritingFlag();
	bool	connectionExpired(int timeoutSec);
	int	getClientOpenFd();
	int	getPortTriggered();
	std::string	getDomainTriggered();
	RequestParse	*getClientRequest();
	Response		*getClientResponse();
	File			*getClientFile();
	void	readRequest(int client_socket);
	void	handle_connect(int client_socket);
};

void	new_connection(std::vector<Client*> &clientList, std::vector<int> &errorFds, Server &server, int serverFd);

#endif