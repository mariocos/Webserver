#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../parse_request/RequestParse.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "File.hpp"
# include "WebSocket.hpp"

# define TIMEOUT 5


class RequestParse;
class Response;
class Server;
class ServerBlock;
class File;
class Cgi;

class Client : public WebSocket
{
private:
	int				_clientPort;
	std::string		_clientIp;
	int				_portTriggered;
	std::string		_domainTriggered;
	RequestParse	*_request;
	Response		*_response;
	File			*_file;
	Cgi				*_cgi;
	ServerBlock		*_serverBlockTriggered;
	bool			_pending;
	bool			_keepAlive;
	int				_openFd;
	uint64_t		_time;
	bool			_finishedReading;
	bool			_finishedWriting;
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
	void	setClientCgi(Cgi *cgi);
	void	setServerBlockTriggered(ServerBlock *serverBlock);
	void	setPortTriggered(int port);
	void	setDomainTriggered(std::string name);
	void	setClientPort(int port);
	void	setClientIp(std::string ip);
	void	resetTimer();
	bool	getClientPending();
	bool	getClientConnection();
	bool	getClientReadingFlag();
	bool	getClientWritingFlag();
	bool	hasToSendToCgi();
	bool	hasTimedOut();
	int		getClientOpenFd();
	int		getPortTriggered();
	int		getClientPort();
	std::string	getClientIp();
	std::string	getDomainTriggered();
	RequestParse	*getClientRequest();
	Response		*getClientResponse();
	File			*getClientFile();
	Cgi				*getClientCgi();
	ServerBlock		*getServerBlockTriggered();
	void	readRequest(int client_socket);
};

void	new_connection(std::vector<Client*> &clientList, std::vector<int> &errorFds, Server &server, int serverFd);
void	clearClient(std::vector<Client*>::iterator	it, std::vector<Client*> &clientList);

#endif