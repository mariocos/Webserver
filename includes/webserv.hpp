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
# include <sys/time.h>
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
# include <map>

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

# include "Client.hpp"
# include "../parse_request/RequestParse.hpp"
# include "../Post_Dele/Post_master.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "Errors.hpp"
# include "File.hpp"
# include "WebSocket.hpp"
# include "ServerBlock.hpp"
# include "Cgi.hpp"
# include "Routes.hpp"
# include "../yaml_parser.hpp"

extern bool	run;

class RequestParse;
class Response;

int		returnVariableType(std::string &value);

template <typename T>
std::string	transformToString(const T &value)
{
	std::ostringstream	ostream;
	std::string			lenght;

	ostream << value;
	lenght = ostream.str();
	return (lenght);
}

template <typename I>
I	transformStringToSomething(std::string &value)
{
	int	check = returnVariableType(value);
	switch (check)
	{
		case 0:
			if (value == "true" || value == "on" || value == "yes")
				return (true);
			else
				return (false);
		case 1:
			return (std::atoi(value.c_str()));
		default:
			return (value);
	}
}

//main.cpp
void	error_connection_handler(Server &server);
void	stopRunning(int signal);;

//printLog.cpp
void	printLog(std::string action, ServerBlock *serverBlock, Client *client, Response *response, int mode);

//utils.cpp
std::string	getTimeStamp();
std::string	convertIpToString(struct in_addr s_addr);
bool		checkInt(const std::string &str, size_t len);
int			returnVariableType(std::string &value);
void		ft_bzero(void *s, size_t n);
int			getMethodRequestedAsInt(std::string method);

//checkConnection.cpp
bool	isConnectionGood(Server &server, std::vector<Client*>::iterator it);
bool	doesPortsMatch(Server &server, std::vector<Client*>::iterator it);
void	handlePortOrDomainMismatch(Server &server, std::vector<Client*>::iterator it);
void	searchForTimeOut(Server &server);

//signal.cpp
void	cleaner(Server &server, bool print);
void	cleanerForServerCreation(Server &server, bool print);

//create_response.cpp
std::string	findFileExtension(std::string path);
void	findType(RequestParse *request, Response *response, Client *client);
void	createHeader(RequestParse *request, Response *response, Client *client);
int 	setNonBlocking(int fd);
void	loadPage(int client_socket, Response *response, Client *client);
void	createPostResponse(Client *client, Response *response);
void	createDeleteResponse(Client *client, Response *response);
void	sendMsgToSocket(int client_socket, int lenght, Client *client, Response *response);

//HoleExplorer.cpp
std::vector<Client*>::iterator	getRightHole(Server &server, int event_fd);

class	NoPendingConnectionsException : public std::runtime_error
{
	public:
		NoPendingConnectionsException();
};

class	NewConnectionCreationException : public std::runtime_error
{
	public:
		NewConnectionCreationException(Server &server);
};

class	SendException : public std::runtime_error
{
	public:
		SendException(Client *client, Response *response);
};

class	ReadException : public std::runtime_error
{
	public:
		ReadException(Client *client, Response *response);
};

class	RedirectException : public std::runtime_error
{
	public:
		RedirectException(Server &server, std::vector<Client*>::iterator it);
};

class	NonBlockingException : public std::runtime_error
{
	public:
		NonBlockingException(int fd);
};

class	BadChildException : public std::runtime_error
{
	public:
		BadChildException();
};

class	BadPipeCreationException : public std::runtime_error
{
	public:
		BadPipeCreationException();
};

class	BadClientException : public std::runtime_error
{
	public:
		BadClientException();
};

class	EmptyBufferException : public std::runtime_error
{
	public:
		EmptyBufferException();
};

class	NoUploadPathException : public std::runtime_error
{
	public:
		NoUploadPathException(Server &server, std::vector<Routes*> tmp);
};

#endif