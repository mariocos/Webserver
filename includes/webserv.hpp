#ifndef WEBSERV_HPP
# define WEBSERV_HPP

/* classes */


//c++
# include <iostream>
# include <string>
# include <cstring>
# include <math.h>
# include <cmath>
# include <fstream>
# include <sstream>
# include <memory>
# include <stdexcept>

//c
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <sys/epoll.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <string.h>
# include <netdb.h>
# include <errno.h>
# include <arpa/inet.h>
# include <poll.h>
# include <fcntl.h>
# include <signal.h>
# include <dirent.h>
# include <stdlib.h>
# include <vector>
# include <ctime>

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

# include "Client.hpp"
# include "../parse_request/RequestParse.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "Errors.hpp"
# include "File.hpp"

extern bool	run;

class RequestParse;
class Response;

//main.cpp
void	stopRunning(int signal);
void	ft_bzero(void *s, size_t n);
void	searchDeadConnections(std::vector<Client*> &clientList, Server &server);
void	error_connection_handler(std::vector<int> &errorFds, Server &server);
void	handlePendingConnections(std::vector<Client*> &clientList, Server &server);

//signal.cpp
void	cleaner(Server &server, std::vector<Client*> &clientList);

//create_response.cpp
void	findType(RequestParse *request, Response *response);
void	createHeader(RequestParse *request, Response *response, Client *client);
int 	setNonBlocking(int fd);
void	loadPage(int client_socket, unsigned int buffer_size, Response *response, Client *client);
void	sendMsgToSocket(int client_socket, int lenght, Client *client, Response *response);

//HoleExplorer.cpp
std::vector<Client*>::iterator	getRightHole(std::vector<Client*> &clientList, int event_fd);
std::vector<Client*>::iterator	getPendingHole(std::vector<Client*> &clientList);
std::vector<Client*>::iterator	getNextPendingHole(std::vector<Client*> &clientList, std::vector<Client*>::iterator it);

class	NoPendingConnectionsException : public std::runtime_error
{
	public:
		NoPendingConnectionsException();
};

class	NewConnectionCreationException : public std::runtime_error
{
	public:
		NewConnectionCreationException(Server &server, std::vector<Client*> &clientList);
};

class	SendException : public std::runtime_error
{
	public:
		SendException(Client *client, Response *response);
};

#endif