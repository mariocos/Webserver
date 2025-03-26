#ifndef ERRORS_HPP
# define ERRORS_HPP

# include "webserv.hpp"

void	loadError400(int client_socket, Client *client);
void	loadError403();
void	loadError404(int client_socket, Response *response, Client *client);
void	loadError405();
void	loadError503(int error_socket);
void	loadError505();

class	Error400Exception : public std::runtime_error
{
	public:
		Error400Exception(int client_socket, Client *client);
};

class	Error403Exception : public std::runtime_error
{
	public:
		Error403Exception();
};

class	Error404Exception : public std::runtime_error
{
	public:
		Error404Exception(int client_socket, Response *response, Client *client);
};

class	Error405Exception : public std::runtime_error
{
	public:
		Error405Exception();
};

class	Error503Exception : public std::runtime_error
{
	public:
		Error503Exception(Client *errorClient, Server &server);
};

class	Error505Exception : public std::runtime_error
{
	public:
		Error505Exception();
};

#endif