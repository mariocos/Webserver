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

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

# include "Client.hpp"
# include "ConfigParser.hpp"
# include "parse_request/RequestParse.hpp"
# include "Response.hpp"

extern int serverskt;

class RequestParse;
class Response;

//main.cpp
void	check(int algo);
int		setup(short port, int backlog);

//signal.cpp
void	ctrl_c(int signal, siginfo_t *info, void *context);
void	ignore(struct sigaction *sa, int signal);
void	signal_decider(int type);

void	loadImgResponse(int client_socket, Response *response, Client *client);
void	loadErrorPage(int client_socket, Response *response, Client *client);
void	loadPage(int client_socket, int fd, Response *response, Client *client);
int 	setNonBlocking(int fd);
void	createHeader(RequestParse *request, Response *response, Client *client);
void	findType(RequestParse *request, Response *response);

int	getNewHole(Client **clients);
int	getRightHole(Client **clients, int event_fd);
int	getPendingHole(Client **clients);
int	getNextPendingHole(Client **clients, int i);
int	findEventFd(Client *clients, epoll_event *events);

#endif