#include "includes/Server.hpp"

Server::Server()
{
}

//Routes* routeType(YamlMap* routeSettings)
//{
//
//}

Routes* routeFromYaml(YamlMap* routeConfig)
{
	int		maxBodySize = -1;
	bool	defaultRoute = true;
	bool 	getMeth = false;
	bool	postMeth = false;
	bool	deleteMeth = false;
	bool	dirList = false;

	YamlList* methods = (YamlList*)routeConfig->getMap().at("methods");
	std::vector<YamlNode*>::iterator itMethods;
	for (itMethods = methods->getList().begin(); itMethods != methods->getList().end(); itMethods++) {
		YamlNode *meth = (*itMethods);
		std::string newMeth = ((YamlScalar<std::string>*)meth)->getValue();
		if (newMeth == "GET")
			getMeth = true;
		else if (newMeth == "POST")
			postMeth = true;
		else if (newMeth == "DELETE")
			deleteMeth = true;
	}

	std::string root = "website";
	std::string path = "";
	std::string cgiPath = "";
	std::string index = "";
	std::string rDirURI = "";
	std::string rDirType = "";
	YamlScalar<std::string>* type;
	YamlList* modules = (YamlList*)routeConfig->getMap().at("modules");
	std::vector<YamlNode*>::iterator itModules;
	for (itModules = modules->getList().begin(); itModules != modules->getList().end(); itModules++) {
		YamlMap* module = (YamlMap*)(*itModules);
		type = (YamlScalar<std::string>*)module->getMap().at("type");
		YamlMap* settings = (YamlMap*)module->getMap().at("settings");
		if (type->getValue() == "static")
		{
//			falta um try aqui
			root = ((YamlScalar<std::string>*)(settings->getMap().at("root")))->getValue();
			dirList = ((YamlScalar<bool>*)(settings->getMap().at("directory_listing")))->getValue(); //Acho que tem de levar um throw por causa do at
//			index = ?;
//			if (postMeth)
//				path = ?;
		}
		else if (type->getValue() == "CGI")
		{
//			root = ((YamlScalar<std::string>*)(settings->getMap().at("root")))->getValue();
//			extensions = ?;
//			index = ?;
//			path = ?
//			if (postMeth)
//				path = ?;
		}
		else if (type->getValue() == "redirect")
		{
//			falta um try aqui
			rDirURI = ((YamlScalar<std::string>*)(settings->getMap().at("uri")))->getValue();
			rDirType = ((YamlScalar<std::string>*)(settings->getMap().at("type")))->getValue();
//			partial = ?;
//			index = ?;
//			if (postMeth)
//				path = ?;
		}
//		YamlMap* settings = (YamlMap*)module->getMap().at("settings");
//		root = ((YamlScalar<std::string>*)(settings->getMap().at("root")))->getValue();
//		dirList = ((YamlScalar<bool>*)(settings->getMap().at("directory_listing")))->getValue(); //Acho que tem de levar um throw por causa do at
	}
	std::string uri = ((YamlScalar<std::string>*)(routeConfig->getMap().at("uri")))->getValue();


	Routes* route = new Routes(maxBodySize, defaultRoute, root, uri);
	if (dirList)
		route->setAsListing();
	route->setMethod(GET, getMeth);
	route->setMethod(POST, postMeth);
	route->setMethod(DELETE, deleteMeth);
	if (type->getValue() == "redirect")
	{
		route->setRedirectPath(rDirURI);
		if (rDirType == "permanent")
			route->setAsPermanentRedirect();
		else
			route->setAsTemporaryRedirect();
	}

	return route;
}

std::vector<Routes*>* routesFromYaml(YamlMap* serverConf)
{
	std::vector<Routes*> *routes = new std::vector<Routes*>;
	YamlList* routesConfig = (YamlList*)serverConf->getMap().at("routes");
	std::vector<YamlNode*>::iterator itRoutes;
	for (itRoutes = routesConfig->getList().begin(); itRoutes != routesConfig->getList().end(); itRoutes++) {
		YamlMap* routeConfig = (YamlMap*)(*itRoutes);
		Routes* route = routeFromYaml(routeConfig);
		routes->push_back(route);
	}

	return routes;
}

ServerBlock* serverBlockFromYaml(YamlMap* serverConf)
{
	int port = ((YamlScalar<int>*)(serverConf->getMap().at("listen")))->getValue();
	int backlog = -1;
	std::string domainName = ((YamlScalar<std::string>*)(serverConf->getMap().at("server_names")))->getValue();
	bool flag = domainName == "localhost";

	std::vector<Routes*> *routes = routesFromYaml(serverConf);
//	set error pages here ServerBlock::setErrorPage ou no startServerBlock mas precisa de mais coisas
//	set Mac Body Size with Routes::setMaxBodySize, have to go throught all the routes to set the size
//	possivelmente aqui é onde tenho de verificar quantos serveres tenho na mesma porta
	int serverSock = socket(AF_INET, SOCK_STREAM, 0);

	ServerBlock* newServerBlock = new ServerBlock(serverSock, port, backlog, domainName, flag);
	newServerBlock->setBlockRoutes(*routes);

	return newServerBlock;
}

YamlNode* getFromYamlMap(YamlNode* node, std::string key)
{
	return ((YamlMap*) node)->getMap().at(key);
}

void Server::startServerBlock(ServerBlock* newServerBlock)
{
	int	option = 1;
	struct sockaddr_in servaddr;
	struct epoll_event	event;

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(newServerBlock->getBlockPort());
	if (setsockopt(newServerBlock->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1 \
		|| bind(newServerBlock->getSocketFd(), (const sockaddr *)&servaddr, sizeof(servaddr)) == -1 \
		|| listen(newServerBlock->getSocketFd(), newServerBlock->getBlockMaxConnections()) == -1)
	{
		delete newServerBlock;
		close(this->_epoll_fd);
		throw SocketBindException(*this);
	}
	event.events = EPOLLIN;
	event.data.fd = newServerBlock->getSocketFd();
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
	{
		delete newServerBlock;
		close(this->_epoll_fd);
		throw EpollCtlException(*this);
	}

	// TODO set the error pages defined in the config file
	newServerBlock->setErrorPage(400, "website/400.html");
	newServerBlock->setErrorPage(403, "website/403.html");
	newServerBlock->setErrorPage(404, "website/404.html");
	newServerBlock->setErrorPage(405, "website/405.html");
	newServerBlock->setErrorPage(413, "website/413.html");
}

Server::Server(YamlNode *parsedConf) : _maxEvents(10)
{
	ServerBlock			*newServerBlock;

	this->_epoll_fd = epoll_create(1);
	if (this->_epoll_fd == -1)
		throw EpollCreationException();

	YamlList* serverConfList = (YamlList*) getFromYamlMap(parsedConf, "servers");

	std::vector<YamlNode*>::iterator it;
	for (it = serverConfList->getList().begin(); it != serverConfList->getList().end(); it++) {
		YamlMap* serverConf = (YamlMap*)(*it);
		newServerBlock = serverBlockFromYaml(serverConf);
		startServerBlock(newServerBlock);
		printLog("INFO", newServerBlock, NULL, NULL, 0, "");
		this->_serverBlocks.push_back(newServerBlock);
	}

	this->_events = new epoll_event[this->_maxEvents];
}

Server::Server(std::vector<int> ports, std::vector<std::string> names, int backlog) : _maxEvents(backlog)
{
	int	option = 1;
	struct sockaddr_in servaddr;
	struct epoll_event	event;
	ServerBlock	*newServerBlock;
	Routes		*newRoute;

	this->_epoll_fd = epoll_create(1);
	if (this->_epoll_fd == -1)
		throw EpollCreationException();
	/*
	Pseudo Code
	while (a < port.size())
	{
		newServerBlock = new ServerBlock();
		std::vector<Routes>	tmpRoutes;
		while (i < list.size)
			tmpRoutes.assing_back(Routes());
		newServerBlock->setBlockRoutes(tmpRoutes);
	}
	 */

	for (size_t i = 0; i < ports.size(); i++)
	{
		std::vector<Routes*>	tmp;
		if (names[i] == "localhost")
			newServerBlock = new ServerBlock(socket(AF_INET, SOCK_STREAM, 0), ports[i], -1, names[i], true);
		else
			newServerBlock = new ServerBlock(socket(AF_INET, SOCK_STREAM, 0), ports[i], -1, names[i], false);
		if (newServerBlock->getSocketFd() == -1)
		{
			delete newServerBlock;
			close(this->_epoll_fd);
			throw SocketCreationException(*this);
		}
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(newServerBlock->getBlockPort());
		if (setsockopt(newServerBlock->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1 \
			|| bind(newServerBlock->getSocketFd(), (const sockaddr *)&servaddr, sizeof(servaddr)) == -1 \
			|| listen(newServerBlock->getSocketFd(), newServerBlock->getBlockMaxConnections()) == -1)
		{
			delete newServerBlock;
			close(this->_epoll_fd);
			throw SocketBindException(*this);
		}
		event.events = EPOLLIN;
		event.data.fd = newServerBlock->getSocketFd();
		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
		{
			delete newServerBlock;
			close(this->_epoll_fd);
			throw EpollCtlException(*this);
		}

		// TODO set the error pages defined in the config file
		newServerBlock->setErrorPage(400, "website/400.html");
		newServerBlock->setErrorPage(403, "website/403.html");
		newServerBlock->setErrorPage(404, "website/404.html");
		newServerBlock->setErrorPage(405, "website/405.html");
		newServerBlock->setErrorPage(413, "website/413.html");

		// TODO implement this as long as the routes list size
		// for (size_t n = 0; i < list.size(); n++)
		// {
		// 		newRoute = new Routes(-1, false, "/", "/");
		// 		tmp.push_back(newRoute);
		// }
		// newServerBlock->setBlockRoutes(tmp);
		// this->_serverBlocks.push_back(newServerBlock);

		// PUT THIS inside the for loop when i get the route list size
		for (size_t n = 0; n < 5; n++)
		{
			if (n == 0)
			{
				newRoute = new Routes(-1, true, "website", "/");
				newRoute->setMethod(GET, true);
				newRoute->setMethod(POST, true);
				newRoute->setMethod(DELETE, true);
				std::string	file = "dummy.html";
				std::string	uploadPath = "./upload";
				newRoute->setDefaultFileForDirectory(file);
				newRoute->setUploadPath(uploadPath);
			}
			else if (n == 1)
			{
				newRoute = new Routes(-1, false, "website/cgi-bin", "/find-this");
				newRoute->setMethod(GET, true);
				newRoute->setMethod(POST, true);
				newRoute->setMethod(DELETE, false);
				std::string	extension = "py";
				std::string	uploadPath = "./upload";
				newRoute->setCgiFileExtension(extension);
				newRoute->setUploadPath(uploadPath);
			}
			else if (n == 2)
			{
				newRoute = new Routes(-1, false, "/home/pauberna/Desktop/projetos_42", "/projects/");
				newRoute->setAsListing();
				newRoute->setMethod(GET, true);
			}
			else if (n == 3)
			{
				newRoute = new Routes(-1, false, "website", "/google");
				newRoute->setAsPermanentRedirect();
				std::string path = "https://google.com";
				newRoute->setRedirectPath(path);
				newRoute->setMethod(GET, true);
			}
			else
			{
				newRoute = new Routes(-1, false, "website", "/redirect");
				newRoute->setAsTemporaryRedirect();
				std::string path = "https://youtube.com";
				newRoute->setRedirectPath(path);
				newRoute->setMethod(GET, true);
			}
			if (ports[i] == 2424 && n != 0)
				newRoute->setAsCgi();
			tmp.push_back(newRoute);
			if (newRoute->canDoMethod(POST) && !newRoute->isCgi() && newRoute->getUploadPath().empty())
			{
				delete newServerBlock;
				throw NoUploadPathException(*this, tmp);
			}
		}
		newServerBlock->setBlockRoutes(tmp);
		printLog("INFO", newServerBlock, NULL, NULL, 0, "");
		printLog("INFO", newServerBlock, NULL, NULL, 1, "");
		this->_serverBlocks.push_back(newServerBlock);
	}
	this->_events = new epoll_event[this->_maxEvents];
}

Server::~Server()
{
	if (this->_events)
		delete[] this->_events;
	close(this->_epoll_fd);
}

void	Server::setEpollCount(int count)
{
	this->_epoll_count = count;
}

int	Server::getEpollFd()
{
	return (this->_epoll_fd);
}

int	Server::getEpollCount()
{
	return (this->_epoll_count);
}

int	Server::getMaxEvents()
{
	return (this->_maxEvents);
}

int	Server::getServerSocketTriggered(int fd)
{
	std::vector<ServerBlock*>::iterator	it = this->_serverBlocks.begin();
	while (it != this->_serverBlocks.end())
	{
		if (fd == (*it)->getSocketFd())
			return (fd);
		it++;
	}
	return (-1);
}

std::vector<Routes*>::iterator	Server::getRouteTriggered(std::string uri, int fd)
{
	std::vector<ServerBlock*>::iterator	it = this->getServerBlockTriggered(fd);
	std::vector<Routes*>::iterator	routeIt = (*it)->getRoutesVector().begin();
	while (routeIt != (*it)->getRoutesVector().end())
	{
		if ((*routeIt)->getURI().rfind('/') == (*routeIt)->getURI().length() - 1 && uri == (*routeIt)->getURI())
			return (routeIt);
		else if ((*routeIt)->getURI().rfind('/') != (*routeIt)->getURI().length() - 1)
		{
			if (uri == (*routeIt)->getURI() || uri == (*routeIt)->getURI() + "/")
				return (routeIt);
		}
		routeIt++;
	}
	return (this->getDefaultRoute(fd));
}

std::vector<Routes*>::iterator	Server::getDefaultRoute(int fd)
{
	std::vector<ServerBlock*>::iterator	it = this->getServerBlockTriggered(fd);
	if (it != this->getServerBlocks().end())
	{
		std::vector<Routes*>::iterator	routeIt = (*it)->getRoutesVector().begin();
		while (routeIt != (*it)->getRoutesVector().end())
		{
			if ((*routeIt)->isDefault())
				return (routeIt);
			routeIt++;
		}
		return ((*it)->getRoutesVector().end());
	}
	return ((*this->getDefaultServerBlock())->getDefaultRoute());
}

std::vector<ServerBlock*>::iterator	Server::getServerBlockTriggered(int fd)
{
	std::vector<ServerBlock*>::iterator	it = this->_serverBlocks.begin();
	while (it != this->_serverBlocks.end())
	{
		if (fd == (*it)->getSocketFd())
			return (it);
		it++;
	}
	return (this->_serverBlocks.end());
}

std::vector<ServerBlock*>::iterator	Server::getDefaultServerBlock()
{
	std::vector<ServerBlock*>::iterator	it = this->_serverBlocks.begin();
	while (it != this->_serverBlocks.end())
	{
		if ((*it)->isDefault())
			return (it);
		it++;
	}
	return (this->_serverBlocks.end());
}

std::vector<ServerBlock*>	Server::getServerBlocks()
{
	return (this->_serverBlocks);
}

ServerBlock	Server::getServerBlock(int index)
{
	return (*this->_serverBlocks[index]);
}

std::vector<Client*>	&Server::getClientListVector()
{
	return (this->_clientList);
}

std::vector<int>	&Server::getErrorFdsVector()
{
	return (this->_errorFds);
}

epoll_event	*Server::getEpollEventArray()
{
	return (this->_events);
}

epoll_event	&Server::getEpollIndex(int index)
{
	return (this->_events[index]);
}

void	Server::removeFromEpoll(int fd)
{
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void	Server::handle_connections()
{
	//handling all the triggers made in epoll_wait
	for (int i = 0; i < this->_epoll_count; i++)
	{
		error_connection_handler(*this);
		struct epoll_event	&event = this->_events[i];
		if (this->getServerSocketTriggered(event.data.fd) != -1)
		{
			try
			{
				new_connection(*this, this->getServerSocketTriggered(event.data.fd));
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		else
			this->manageConnection(event);
	}
}

void	Server::manageConnection(epoll_event &event)
{
	std::vector<Client*>::iterator	it;

	it = isThisPipe(event.data.fd, *this);
	if (it != this->_clientList.end())
	{
		try
		{
			//in case the fd triggered is from a pipe
			cgiHandler(*this, (*it), event.data.fd);
			return;
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) == "Bad child")
				throw BadChildException();
			if (std::string(e.what()) != "BadClient")
				std::cerr << e.what() << '\n';
			clearClient(it, *this);
		}
	}
	//getting from the clientList which client was triggered/disconnected
	it = getRightHole(*this, event.data.fd);
	if (it == this->_clientList.end())
		return;
	else if ((event.events == EPOLLRDHUP) && \
		(*it)->getClientReadingFlag() && (*it)->getClientWritingFlag() && (*it)->getClientOpenFd() == -1)
		clearClient(it, *this);
	else if (!(*it)->getClientReadingFlag())
	{
		try
		{
			if (event.events == EPOLLIN && !(*it)->getClientPending())
			{
				(*it)->readRequest((*it)->getSocketFd());
				if ((*it)->getClientReadingFlag())
					(*it)->setClientWritingFlag(false);
				(*it)->resetTimer();
				(*it)->setClientPending(true);
				(*it)->setSocketToWriting(this->_epoll_fd);
				(*it)->setRouteTriggered((*this->getRouteTriggered((*it)->getURIRequested(), (*it)->getSocketTriggered())));
			}
			else if (event.events == EPOLLOUT)
			{
				//in case the body is still to be sent
				send((*it)->getSocketFd(), "HTTP/1.1 100 Continue\r\n\r\n", 25, MSG_NOSIGNAL);
				(*it)->setSocketToReading(this->_epoll_fd);
			}
			else if ((*it)->getClientPending())
				(*it)->readBodyOfRequest(*this, (*it)->getClientRequest());
			
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) == "Expectation" || std::string(e.what()) == "HTTPVersion")
			{
				if ((*it)->getClientRequest()->get_connection() == "keep-alive")
					(*it)->resetClient(*this);
				else
					clearClient(it, *this);
			}
			else if (std::string(e.what()) != "EmptyBuffer")
				std::cerr << e.what() << '\n';
		}
	}
	else
		this->manageClient(it, event);
}

void	Server::manageClient(std::vector<Client*>::iterator it, epoll_event	&event)
{
	if (!isConnectionGood(*this, it))
		handlePortOrDomainMismatch(*this, it);
	else if ((*it)->getRouteTriggered()->isCgi())
	{
		try
		{
			//handling the CGI before and after the child process is created
			if ((*it)->getClientCgi() && (*it)->getClientCgi()->isActive())
				return ;
			cgiHandler(*this, (*it), event.data.fd);
			if ((*it)->getClientWritingFlag() && (*it)->getClientReadingFlag())
			{
				(*it)->resetClient(*this);
				delete (*it)->getClientCgi();
				(*it)->setClientCgi(NULL);
			}
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) == "Bad child")
				throw BadChildException();
			if (std::string(e.what()) != "BadClient")
				std::cerr << e.what() << '\n';
			clearClient(it, *this);
		}
	}
	else
	{
		try
		{
			if (event.events == EPOLLOUT)
				(*it)->getClientRequest()->execute_response((*it)->getSocketFd(), (*it));
			else
				return ;
			if (!(*it)->getClientFile()->isReading() && !(*it)->getClientFile()->isWriting())
				(*it)->resetClient(*this);
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) == "Redirect")
			{
				if ((*it)->getClientRequest()->get_connection() == "keep-alive")
					(*it)->resetClient(*this);
				else
					clearClient(it, *this);
			}
			else if (std::string(e.what()) != "Loading Listing")
			{
				std::cerr << e.what() << '\n';
				clearClient(it, *this);
			}
			else if (!(*it)->getClientFile()->getFile()->is_open())
				(*it)->resetClient(*this);
		}
	}
}

void	Server::addClientToClientVector(Client *client)
{
	this->_clientList.push_back(client);
}

void	Server::removeClientFromClientVector(std::vector<Client*>::iterator client)
{
	this->_clientList.erase(client);
}

void	Server::addErrorFdToErrorVector(int fd)
{
	this->_errorFds.push_back(fd);
}

void	Server::removeErrorFdFromErrorVector(std::vector<int>::iterator errorFd)
{
	this->_errorFds.erase(errorFd);
}

Server::SocketCreationException::SocketCreationException(Server &server) :
runtime_error(RED"Error creating the socket"RESET)
{
	cleaner(server, true);
}

Server::SocketBindException::SocketBindException(Server &server) :
runtime_error(RED"Error binding the socket"RESET)
{
	cleaner(server, true);
}

Server::EpollCreationException::EpollCreationException() :
runtime_error(RED"Error creating the epoll_fd"RESET) {}

Server::EpollCtlException::EpollCtlException(Server &server) :
runtime_error(RED"Error managing the epoll"RESET)
{
	cleaner(server, true);
}

Server::NoFileToReadException::NoFileToReadException() :
runtime_error(RED"No File Pending"RESET) {}
