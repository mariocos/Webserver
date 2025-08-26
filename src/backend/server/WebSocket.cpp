#include "../../../includes/WebSocket.hpp"

WebSocket::WebSocket() : _socket(-1), _state(INACTIVE)
{
}

WebSocket::WebSocket(int socket) : _socket(socket), _state(READING)
{
}

WebSocket::WebSocket(const WebSocket &copy)
{
	*this = copy;
}

WebSocket	&WebSocket::operator=(const WebSocket &copy)
{
	if (this != &copy)
	{
		this->_socket = copy._socket;
		this->_state = copy._state;
	}
	return (*this);
}

WebSocket::~WebSocket()
{
	if (this->_socket != -1)
		close(this->_socket);
}

int	WebSocket::getSocketFd()
{
	return (this->_socket);
}

int	WebSocket::getSocketState()
{
	return (this->_state);
}

void	WebSocket::setSocketFd(int fd)
{
	this->_socket = fd;
}

void	WebSocket::setSocketState(int state)
{
	this->_state = state;
}

void	WebSocket::setSocketToReading(int epollFd)
{
	struct epoll_event	event;
	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = this->_socket;
	epoll_ctl(epollFd, EPOLL_CTL_MOD, event.data.fd, &event);
	this->_state = READING;
}

void	WebSocket::setSocketToWriting(int epollFd)
{
	struct epoll_event	event;
	event.events = EPOLLOUT | EPOLLRDHUP;
	event.data.fd = this->_socket;
	epoll_ctl(epollFd, EPOLL_CTL_MOD, event.data.fd, &event);
	this->_state = WRITING;
}

void	WebSocket::setSocketToEdge(int epollFd)
{
	struct epoll_event	event;
	event.events = EPOLLET | EPOLLRDHUP;
	event.data.fd = this->_socket;
	epoll_ctl(epollFd, EPOLL_CTL_MOD, event.data.fd, &event);
	this->_state = EDGE;
}

void	WebSocket::addSocketToEpoll(int epollFd)
{
	struct epoll_event	event;
	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = this->_socket;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, event.data.fd, &event);
	this->_state = READING;
}

void	WebSocket::removeSocketFromEpoll(int epollFd)
{
	epoll_ctl(epollFd, EPOLL_CTL_DEL, this->_socket, NULL);
	this->_state = INACTIVE;
}
