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

	this->_epoll_fd = epoll_create(1);
	if (this->_epoll_fd == -1)
		throw EpollCreationException();
	for (size_t i = 0; i < ports.size() && i < names.size(); i++)
	{
		//hardcoded, need to receive from parsing which is default
		if (names[i] == "localhost")
			newServerBlock = new ServerBlock(ports[i], backlog, names[i], socket(AF_INET, SOCK_STREAM, 0), true);
		else
			newServerBlock = new ServerBlock(ports[i], backlog, names[i], socket(AF_INET, SOCK_STREAM, 0), false);
		if (newServerBlock->getSocketFd() == -1)
		{
			delete newServerBlock;
			close(this->_epoll_fd);
			throw SocketCreationException();
		}
		if (ports[i] == 2424)
			newServerBlock->setBlockAsCgi();
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
		this->_serverBlocks.push_back(newServerBlock);
		printLog("INFO", newServerBlock, NULL, NULL, 0);
		printLog("INFO", newServerBlock, NULL, NULL, 1);
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
	std::vector<Client*>::iterator	it;
	//handling all the triggers made in epoll_wait
	for (int i = 0; i < this->_epoll_count; i++)
	{
		struct epoll_event	&event = this->_events[i];
		error_connection_handler(errorFds, *this);
		if (this->getServerSocketTriggered(event.data.fd) != -1)
		{
			try
			{
				//accepting new connections using the corresponding socket triggered
				new_connection(clientList, errorFds, *this, this->getServerSocketTriggered(event.data.fd));
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		else
		{
			//checking if the fd triggered was a fd from a pipe of a cgi
			it = isThisPipe(event.data.fd, clientList);
			if (it != clientList.end())
			{
				cgiHandler(*this, (*it));
				continue;
			}
			//getting from the clientList which client was triggered/disconnected
			it = getRightHole(clientList, event.data.fd);
			if (it == clientList.end())
				continue;
			else if (event.events & EPOLLRDHUP)
				clearClient(it, clientList);
			else
			{
				//reading the request received
				(*it)->readRequest((*it)->getSocketFd());
				(*it)->setClientWritingFlag(false);
				(*it)->setSocketToReading(this->_epoll_fd);
			}
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
