#include "includes/Client.hpp"

Client::Client() : WebSocket(-1), _request(NULL), _response(NULL), _pending(false), _keepAlive(false), _openFd(-1), _socketTriggered(-1), _finishedReading(false), _finishedWriting(true)
{
}

Client::Client(int client_socket) : WebSocket(client_socket), _request(NULL), _response(NULL), _pending(false), _keepAlive(false),_openFd(-1), _socketTriggered(-1), _finishedReading(false), _finishedWriting(true)
{
	if (setNonBlocking(this->getSocketFd()) == -1)
	{
		std::cerr << "Failed to set non-blocking mode." << std::endl;
		close(this->getSocketFd());
		this->setSocketFd(-1);
	}
	this->_response = new Response;
	this->_request = new RequestParse;
	this->_file = new File;
	this->_file->setClient(this);
	std::string *buffer = new std::string;
	this->_response->setBuffer(buffer);
	this->_request->setBuffer(buffer);
	this->_cgi = NULL;
	this->_serverBlockTriggered = NULL;
	this->_routeTriggered = NULL;
	this->resetTimer();
}

Client::~Client()
{
	delete this->_response->getBuffer();
	delete this->_request;
	delete this->_response;
	delete this->_file;
	if (this->_cgi)
		delete this->_cgi;
	if (this->_openFd != -1)
		close(this->_openFd);
}

void	Client::setClientPending(bool pending)
{
	this->_pending = pending;
}

void	Client::setClientConnection(bool connection)
{
	this->_keepAlive = connection;
}

void	Client::setClientRequest(RequestParse *request)
{
	this->_request = request;
}

void	Client::setClientResponse(Response *response)
{
	this->_response = response;
}

void	Client::setClientFile(File *file)
{
	this->_file = file;
}

void	Client::setClientCgi(Cgi *cgi)
{
	this->_cgi = cgi;
}

void	Client::setRouteTriggered(Routes *route)
{
	this->_routeTriggered = route;
}

void	Client::setServerBlockTriggered(ServerBlock *serverBlock)
{
	this->_serverBlockTriggered = serverBlock;
}

void	Client::setClientOpenFd(int fd)
{
	this->_openFd = fd;
}

void	Client::setClientReadingFlag(bool flag)
{
	this->_finishedReading = flag;
}

void	Client::setClientWritingFlag(bool flag)
{
	this->_finishedWriting = flag;
}

void	Client::setPortTriggered(int port)
{
	this->_portTriggered = port;
}

void	Client::setDomainTriggered(std::string name)
{
	this->_domainTriggered = name;
}

void	Client::setClientPort(int port)
{
	this->_clientPort = port;
}

void	Client::setClientIp(std::string ip)
{
	this->_clientIp = ip;
}

void	Client::resetTimer()
{
	struct timeval	tv;
	gettimeofday(&tv, NULL);
	this->_time = (uint64_t)tv.tv_sec;
}

void	Client::setSocketTriggered(int fd)
{
	this->_socketTriggered = fd;
}

bool	Client::getClientPending()
{
	return (this->_pending);
}

bool	Client::getClientConnection()
{
	return (this->_keepAlive);
}

bool	Client::getClientReadingFlag()
{
	return (this->_finishedReading);
}

bool	Client::getClientWritingFlag()
{
	return (this->_finishedWriting);
}

bool	Client::hasToSendToCgi()
{
	std::string	cgiFolder = get_keyword(this->_request->get_path(), "/cgi-bin/");
	if (cgiFolder.empty())
		return (false);
	return (true);
}

bool	Client::hasTimedOut()
{
	struct timeval	tv;
	gettimeofday(&tv, NULL);
	uint64_t	actualTime = (uint64_t)tv.tv_sec;
	if (actualTime - this->_time >= TIMEOUT)
		return (true);
	return (false);
}

int	Client::getClientOpenFd()
{
	return (_openFd);
}

int	Client::getPortTriggered()
{
	return (this->_portTriggered);
}

int	Client::getClientPort()
{
	return (this->_clientPort);
}

int	Client::getSocketTriggered()
{
	return (this->_socketTriggered);
}

std::string	Client::getClientIp()
{
	return (this->_clientIp);
}

std::string	Client::getDomainTriggered()
{
	return (this->_domainTriggered);
}

std::string	Client::getNewPath()
{
	std::string path;
	if (this->_routeTriggered->getURI() != "/")
	{
		std::string newPath = this->_request->get_path().substr(this->_routeTriggered->getURI().length());
		path = this->_routeTriggered->getRoot() + "/" + newPath;
	}
	else
		path = this->_routeTriggered->getRoot() + this->_request->get_path();
	return (path);
}

std::string	Client::getURIRequested()
{
	std::string	uri;
	if (this->_request->get_path().find('/', 1) != std::string::npos)
		uri = this->_request->get_path().substr(0, this->_request->get_path().find('/', 1) + 1);
	else
		uri = this->_request->get_path();
	return (uri);
}

RequestParse	*Client::getClientRequest()
{
	return (this->_request);
}

Response	*Client::getClientResponse()
{
	return (this->_response);
}

File	*Client::getClientFile()
{
	return (this->_file);
}

Cgi		*Client::getClientCgi()
{
	return (this->_cgi);
}

ServerBlock	*Client::getServerBlockTriggered()
{
	return (this->_serverBlockTriggered);
}

Routes	*Client::getRouteTriggered()
{
	return (this->_routeTriggered);
}

void	Client::readRequest(int client_socket)
{
	this->getClientRequest()->readBinary(client_socket, this);
	this->getClientRequest()->buildRequest(reinterpret_cast<char*>(this->getClientRequest()->getBufferInfo().data()));
}

void	Client::resetClient(Server &server)
{
	this->setClientWritingFlag(false);
	this->setClientReadingFlag(false);
	this->setClientPending(false);
	this->getClientFile()->setReading(false);
	this->getClientFile()->setWriting(false);
	this->getClientResponse()->resetResponseLenght();
	this->getClientResponse()->resetBytesToSend();
	this->getClientResponse()->resetBytesSent();
	this->setSocketToReading(server.getEpollFd());
}

void	new_connection(std::vector<Client*> &clientList, std::vector<int> &errorFds, Server &server, int serverFd)
{
	struct sockaddr_in clientaddr;
	socklen_t	addrlen = sizeof(clientaddr);
	Client	*newClient = new Client(accept(serverFd, (struct sockaddr*)&clientaddr, &addrlen));
	if (newClient->getSocketFd() == -1)
		throw NewConnectionCreationException(server, clientList);
	newClient->setClientIp(convertIpToString(clientaddr.sin_addr));
	newClient->setClientPort(ntohs(clientaddr.sin_port));
	//still kinda hardcoded to store the information about the server block triggered
	std::vector<ServerBlock*>::iterator	it = server.getServerBlockTriggered(serverFd);
	newClient->setPortTriggered((*it)->getBlockPort());
	newClient->setDomainTriggered((*it)->getBlockName());
	newClient->setServerBlockTriggered((*it));
	newClient->setSocketTriggered(serverFd);
	newClient->addSocketToEpoll(server.getEpollFd());
	if (clientList.size() < 3000)
		clientList.push_back(newClient);
	else
	{
		printLog("ERROR", NULL, newClient, NULL, 503);
		errorFds.push_back(newClient->getSocketFd());
		newClient->setSocketFd(-1);
		delete	newClient;
		return ;
	}
	printLog("INFO", NULL, newClient, NULL, 3);
}

void	clearClient(std::vector<Client*>::iterator	it, std::vector<Client*> &clientList)
{
	printLog("INFO", NULL, (*it), NULL, 4);
	(*it)->setClientOpenFd(-1);
	(*it)->removeSocketFromEpoll((*it)->getSocketFd());
	close((*it)->getSocketFd());
	delete (*it);
	clientList.erase(it);
}
