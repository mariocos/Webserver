#include "includes/Server.hpp"

Server::Server()
{
}

Server::Server(std::vector<int> ports, std::vector<std::string> names, int backlog) : _maxEvents(backlog)
{
	int	option = 1;
	struct sockaddr_in servaddr;
	struct epoll_event	event;

	this->_epoll_fd = epoll_create(1);
	if (this->_epoll_fd == -1)
		throw EpollCreationException();
	for (size_t i = 0; i < ports.size() && i < names.size(); i++)
	{
		ServerBlock	*newServerBlock = new ServerBlock(ports[i], backlog, names[i], socket(AF_INET, SOCK_STREAM, 0));
		if (newServerBlock->getSocketFd() == -1)
			throw SocketCreationException();
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(newServerBlock->getBlockPort());
		if (setsockopt(newServerBlock->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
			throw SocketBindException();
		if (bind(newServerBlock->getSocketFd(), (const sockaddr *)&servaddr, sizeof(servaddr)) == -1)
			throw SocketBindException();
		if (listen(newServerBlock->getSocketFd(), newServerBlock->getBlockMaxConnections()) == -1)
			throw SocketBindException();
		event.events = EPOLLIN;
		event.data.fd = newServerBlock->getSocketFd();
		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
			throw EpollCtlException();
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

void	Server::addNewSocket(int fd)
{
	struct epoll_event	event;
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
	event.data.fd = fd;
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event);
}

void	Server::removeFromEpoll(int fd)
{
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void	Server::handle_connections(std::vector<Client*> &clientList, std::vector<int> &errorFds)
{
	std::vector<Client*>::iterator	it;
	for (int i = 0; i < this->_epoll_count; i++)
	{
		struct epoll_event	&event = this->_events[i];
		error_connection_handler(errorFds, *this);
		if (this->getServerSocketTriggered(event.data.fd) != -1)
		{
			try
			{
				new_connection(clientList, errorFds, *this, this->getServerSocketTriggered(event.data.fd));
				std::cout<<GREEN<<"Connection successuful"<<RESET<<std::endl;
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		else
		{
			it = getRightHole(clientList, event.data.fd);
			if (it == clientList.end())
			{
				std::cout<<RED<<"All spaces ocupied"<<RESET<<std::endl;
				continue;
			}
			else if (event.events & EPOLLRDHUP)
			{
				std::cout<<YELLOW<<"Client Disconnected"<<RESET<<std::endl;
				std::cout<<YELLOW<<"Socket that disconect was: "<<(*it)->getSocketFd()<<RESET<<std::endl;
				this->removeFromEpoll((*it)->getSocketFd());
				delete (*it);
				clientList.erase(it);
			}
			else
			{
				(*it)->readRequest((*it)->getSocketFd());
				(*it)->setClientWritingFlag(false);
			}
		}
	}
}

Server::SocketCreationException::SocketCreationException() :
runtime_error("Error creating the socket") {}

Server::SocketBindException::SocketBindException() :
runtime_error("Error binding the socket") {}

Server::EpollCreationException::EpollCreationException() :
runtime_error("Error creating the epoll_fd") {}

Server::EpollCtlException::EpollCtlException() :
runtime_error("Error managing the epoll") {}

Server::NoFileToReadException::NoFileToReadException() :
runtime_error("No File Pending") {}
