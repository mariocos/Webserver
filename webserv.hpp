#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//c++
# include <iostream>
# include <string>
# include <cstring>
# include <math.h>
# include <cmath>
# include <fstream>
# include <sstream>

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

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

void	ctrl_c(int signal, siginfo_t *info, void *context);
void	ignore(struct sigaction *sa, int signal);
void	signal_decider(int type);
void	check(int algo);
int		setup(short port, int backlog);
int		new_connection(int server_socket);
void	make_response(int client_socket, char *buffer);
void	*handle_connect(int client_socket);

#endif