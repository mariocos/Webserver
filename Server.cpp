#include "Server.hpp"

Server::Server() : _serverSocket(-1)
{
}

Server::Server(int port, int backlog) : _maxEvents(backlog)
{
	int	option = 1;
	struct sockaddr_in servaddr;
	struct epoll_event	event;

	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_serverSocket == -1)
		throw SocketCreationException();
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);
	if (setsockopt(this->_serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, 4) == -1)
		throw SocketBindException();
	if (bind(this->_serverSocket, (const sockaddr *)&servaddr, sizeof(servaddr)) == -1)
		throw SocketBindException();
	if (listen(_serverSocket, backlog) == -1)
		throw SocketBindException();
	this->_epoll_fd = epoll_create(1);
	if (this->_epoll_fd == -1)
		throw EpollCreationException();
	event.events = EPOLLIN;
	event.data.fd = _serverSocket;
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
		throw EpollCtlException();
	this->_events = new epoll_event[this->_maxEvents];
}

Server::~Server()
{
	if (this->_events)
		delete[] this->_events;
}

void	Server::setEpollCount(int count)
{
	this->_epoll_count = count;
}

int	Server::getServerSocket()
{
	return (this->_serverSocket);
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
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = fd;
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event);
}

void	Server::removeFromEpoll(int fd)
{
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void	Server::handle_connections(std::vector<Client*> &clientList, std::vector<int> &errorFds)
{
	if (this->_epoll_count == 0)
		return;
	std::vector<Client*>::iterator	it;
	for (int i = 0; i < this->_epoll_count; i++)
	{
		struct epoll_event	&event = this->_events[i];
		error_connection_handler(errorFds, *this);
		if (event.data.fd == this->_serverSocket)
		{
			if (new_connection(clientList, errorFds, *this) == -1)
				throw NewConnectionCreationException();
			std::cout<<GREEN<<"Connection successuful"<<RESET<<std::endl;
		}
		else
		{
			it = getRightHole(clientList, event.data.fd);
			if (it == clientList.end())
			{
				std::cout<<RED<<"All spaces ocupied"<<RESET<<std::endl;
				continue;
			}
			else if ((*it)->getClientReadingFlag() == false)
			{
				std::cout<<"Reading Request"<<std::endl;
				(*it)->readRequest((*it)->getClientSocket());
				(*it)->setClientWritingFlag(false);
			}
			else
			{
				(*it)->handle_connect(event.data.fd);
				if ((*it)->getClientWritingFlag() == true)
				{
					this->removeFromEpoll((*it)->getClientSocket());
					clientList.erase(it);
				}
			}
		}
	}
}
