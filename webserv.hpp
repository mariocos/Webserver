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

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

# include "Client.hpp"
# include "ConfigParser.hpp"
# include "parse_request/RequestParse.hpp"
# include "Response.hpp"
# include "Server.hpp"

extern bool	run;

class RequestParse;
class Response;

//main.cpp
void	check(int algo);
void	ft_bzero(void *s, size_t n);

//signal.cpp
void	stopRunning(int signal);
void	cleaner(Server &server, std::vector<Client*> &clientList);

void	loadImgResponse(int client_socket, Response *response, Client *client);
void	loadErrorPage(int client_socket, Response *response, Client *client);
void	loadPage(int client_socket, int fd, Response *response, Client *client);
int 	setNonBlocking(int fd);
void	createHeader(RequestParse *request, Response *response, Client *client);
void	findType(RequestParse *request, Response *response);

std::vector<Client*>::iterator	getRightHole(std::vector<Client*> &clientList, int event_fd);
std::vector<Client*>::iterator	getPendingHole(std::vector<Client*> &clientList);
std::vector<Client*>::iterator	getNextPendingHole(std::vector<Client*> &clientList, std::vector<Client*>::iterator it);
void	error_connection_handler(std::vector<int> &errorFds, Server &server);
void	handlePendingConnections(std::vector<Client*> &clientList, Server &server);

class	NoPendingConnectionsException : public std::exception
{
	public:
		virtual const char	*what() const throw()
		{
			return ("Nothing Pending");
		}
};

#endif