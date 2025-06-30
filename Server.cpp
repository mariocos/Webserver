#include "includes/Server.hpp"

Server::Server()
{
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
			newServerBlock = new ServerBlock(socket(AF_INET, SOCK_STREAM, 0), ports[i], backlog, names[i], true);
		else
			newServerBlock = new ServerBlock(socket(AF_INET, SOCK_STREAM, 0), ports[i], backlog, names[i], false);
		if (newServerBlock->getSocketFd() == -1)
		{
			delete newServerBlock;
			close(this->_epoll_fd);
			throw SocketCreationException();
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
			throw SocketBindException();
		}
		event.events = EPOLLIN;
		event.data.fd = newServerBlock->getSocketFd();
		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
			throw EpollCtlException();

		// TODO implement this as long as the routes list size
		// for (size_t n = 0; i < list.size(); n++)
		// {
		// 		newRoute = new Routes(backlog, -1, false, "/", "/");
		// 		tmp.push_back(newRoute);
		// }
		// newServerBlock->setBlockRoutes(tmp);
		// this->_serverBlocks.push_back(newServerBlock);

		// PUT THIS inside the for loop when i get the route list size
		for (size_t n = 0; n < 5; n++)
		{
			if (n == 0)
				newRoute = new Routes(backlog, -1, true, "website", "/");
			else if (n == 1)
				newRoute = new Routes(backlog, -1, false, "website/cgi-bin", "/find-this");
			else if (n == 2)
			{
				newRoute = new Routes(backlog, -1, false, "/home/pbjr/Desktop", "/projects/");
				newRoute->setAsListing();
			}
			else if (n == 3)
			{
				newRoute = new Routes(backlog, -1, false, "website", "/google");
				newRoute->setAsPermanentRedirect();
				std::string path = "https://google.com";
				newRoute->setRedirectPath(path);
			}
			else
			{
				newRoute = new Routes(backlog, -1, false, "website", "/redirect");
				newRoute->setAsTemporaryRedirect();
				std::string path = "https://youtube.com";
				newRoute->setRedirectPath(path);
			}
			if (ports[i] == 2424 && n != 0)
				newRoute->setAsCgi();
			tmp.push_back(newRoute);
		}
		newServerBlock->setBlockRoutes(tmp);
		printLog("INFO", newServerBlock, NULL, NULL, 0);
		printLog("INFO", newServerBlock, NULL, NULL, 1);
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

epoll_event	*Server::getEpollEventArray()
{
	return (this->_events);
}

epoll_event	Server::getEpollIndex(int index)
{
	return (this->_events[index]);
}

void	Server::removeFromEpoll(int fd)
{
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void	Server::handle_connections(std::vector<Client*> &clientList, std::vector<int> &errorFds)
{
	//handling all the triggers made in epoll_wait
	for (int i = 0; i < this->_epoll_count; i++)
	{
		error_connection_handler(errorFds, *this);
		struct epoll_event	&event = this->_events[i];
		if (this->getServerSocketTriggered(event.data.fd) != -1)
		{
			try
			{
				new_connection(clientList, errorFds, *this, this->getServerSocketTriggered(event.data.fd));
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		else
			this->manageConnection(clientList, event);
	}
}

void	Server::manageConnection(std::vector<Client*> &clientList, epoll_event	&event)
{
	std::vector<Client*>::iterator	it;

	//checking if the fd triggered was a fd from a pipe of a cgi
	it = isThisPipe(event.data.fd, clientList);
	if (it != clientList.end())
	{
		try
		{
			cgiHandler(*this, (*it));
			return;
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) == "Bad child")
				throw BadChildException();
			if (std::string(e.what()) != "BadClient")
				std::cerr << e.what() << '\n';
			clearClient(it, clientList);
		}
	}
	//getting from the clientList which client was triggered/disconnected
	it = getRightHole(clientList, event.data.fd);
	if (it == clientList.end())
		return;
	else if ((event.events & EPOLLRDHUP) && \
		(*it)->getClientReadingFlag() && (*it)->getClientWritingFlag() && (*it)->getClientOpenFd() == -1)
		clearClient(it, clientList);
	else if (!(*it)->getClientReadingFlag() && (event.events & EPOLLIN))
	{
		try
		{
			(*it)->readRequest((*it)->getSocketFd());
			if ((*it)->getClientReadingFlag())
			{
				(*it)->resetTimer();
				(*it)->setClientWritingFlag(false);
				(*it)->setSocketToWriting(this->_epoll_fd);
			}
			(*it)->setRouteTriggered((*this->getRouteTriggered((*it)->getURIRequested(), (*it)->getSocketTriggered())));
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) != "EmptyBuffer")
				std::cerr << e.what() << '\n';
		}
	}
	else
		this->manageClient(clientList, it, event);
}

void	Server::manageClient(std::vector<Client*> &clientList, std::vector<Client*>::iterator it, epoll_event	&event)
{
	if (!isConnectionGood(*this, it))
		handlePortOrDomainMismatch(*this, clientList, it);
	else if ((*it)->getRouteTriggered()->isCgi())
	{
		try
		{
			//handling the CGI before and after the child process is created
			if ((*it)->getClientCgi() && (*it)->getClientCgi()->isActive())
				return ;
			cgiHandler(*this, (*it));
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
			clearClient(it, clientList);
		}
	}
	else
	{
		try
		{
			if (event.events & EPOLLOUT)
				(*it)->getClientRequest()->execute_response((*it)->getSocketFd(), (*it));
			else
				return ;
			if (!(*it)->getClientFile()->isReading() && !(*it)->getClientFile()->isWriting())
				(*it)->resetClient(*this);
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) != "Loading Listing")
			{
				std::cerr << e.what() << '\n';
				clearClient(it, clientList);
			}
			else if (!(*it)->getClientFile()->getFile()->is_open())
				(*it)->resetClient(*this);
		}
	}
}

Server::SocketCreationException::SocketCreationException() :
runtime_error(RED"Error creating the socket"RESET) {}

Server::SocketBindException::SocketBindException() :
runtime_error(RED"Error binding the socket"RESET) {}

Server::EpollCreationException::EpollCreationException() :
runtime_error(RED"Error creating the epoll_fd"RESET) {}

Server::EpollCtlException::EpollCtlException() :
runtime_error(RED"Error managing the epoll"RESET) {}

Server::NoFileToReadException::NoFileToReadException() :
runtime_error(RED"No File Pending"RESET) {}
