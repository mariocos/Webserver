#include "Server.hpp"

Server::Server() : _serverSocket(-1)
{
}

Server::Server(int port, int backlog)
{
	int	option = 1;
	struct sockaddr_in servaddr;

	check((_serverSocket = socket(AF_INET, SOCK_STREAM, 0)));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, 4);
	check(bind(_serverSocket, (const sockaddr *)&servaddr, sizeof(servaddr)));
	check(listen(_serverSocket, backlog));
	_epoll_fd = epoll_create(1);
	_event.events = EPOLLIN;
	_event.data.fd = _serverSocket;
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _event.data.fd, &_event);
}

Server::~Server()
{
}

void	Server::setEpollCount(int count)
{
	_epoll_count = count;
}

int	Server::getServerSocket()
{
	return (_serverSocket);
}

int	Server::getEpollFd()
{
	return (_epoll_fd);
}

int	Server::getEpollCount()
{
	return (_epoll_count);
}

epoll_event	Server::getEpollEvent()
{
	return (_event);
}

epoll_event	*Server::getEpollEventArray()
{
	return (_events);
}

epoll_event	Server::getEpollIndex(int index)
{
	return (_events[index]);
}

void	Server::addNewSocket(int fd)
{
	_event.events = EPOLLIN | EPOLLET;
	_event.data.fd = fd;
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _event.data.fd, &_event);
}

void	Server::addToEpollIndex(int fd, int event_number)
{
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &_events[event_number]);
}

void	Server::removeFromEpoll(int fd, int event_number)
{
	epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, &_events[event_number]);
}

int	Server::handle_connections(std::vector<Client*> &clientList, std::vector<int> &errorFds)
{
	std::vector<Client*>::iterator	it;
	for (int i = 0; i < _epoll_count; i++)
	{
		error_connection_handler(errorFds, *this);
		if (_events[i].data.fd == _serverSocket)
		{
			if (new_connection(clientList, errorFds, *this) == -1)
				return (-1);
			std::cout<<GREEN<<"Connection successuful"<<RESET<<std::endl;
		}
		else
		{
			it = getRightHole(clientList, _events[i].data.fd);
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
				(*it)->handle_connect(_events[i].data.fd);
				if ((*it)->getClientWritingFlag() == true)
				{
					this->removeFromEpoll((*it)->getClientSocket(), i);
					clientList.erase(it);
				}
			}
		}
	}
	return (0);
}
