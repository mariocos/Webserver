#ifndef ERRORS_HPP
# define ERRORS_HPP

# include "webserv.hpp"

void	load301(int client_socket, Response *response, Client *client);
void	load307(int client_socket, Response *response, Client *client);
void	loadError400(int client_socket, Response *response, Client *client);
void	loadError403(int client_socket, Response *response, Client *client);
void	loadError404(int client_socket, Response *response, Client *client);
void	loadError405(int client_socket, Response *response, Client *client);
void	loadError408(int client_socket, Response *response, Client *client);
void	loadError409(int client_socket, Response *response, Client *client);
void	loadError413(int client_socket, Response *response, Client *client);
void	loadError417(int client_socket, Response *response, Client *client);
void	loadError503(int error_socket);
void	loadError505(int client_socket, Response *response, Client *client);

class	Load301Exception : public std::runtime_error
{
	public:
		Load301Exception(int client_socket, Response *response, Client *client);
};

class	Load307Exception : public std::runtime_error
{
	public:
		Load307Exception(int client_socket, Response *response, Client *client);
};

class	Error400Exception : public std::runtime_error
{
	public:
		Error400Exception(int client_socket, Response *response, Client *client);
};

class	Error403Exception : public std::runtime_error
{
	public:
		Error403Exception(int client_socket, Response *response, Client *client);
};

class	Error404Exception : public std::runtime_error
{
	public:
		Error404Exception(int client_socket, Response *response, Client *client);
};

class	Error405Exception : public std::runtime_error
{
	public:
		Error405Exception(int client_socket, Response *response, Client *client);
};

class	Error409Exception : public std::runtime_error
{
	public:
		Error409Exception(int client_socket, Response *response, Client *client);
};

class	Error413Exception : public std::runtime_error
{
	public:
		Error413Exception(int client_socket, Response *response, Client *client);
};

class	Error417Exception : public std::runtime_error
{
	public:
		Error417Exception(int client_socket, Response *response, Client *client);
};

class	Error503Exception : public std::runtime_error
{
	public:
		Error503Exception(Client *errorClient, Server &server);
};

class	Error505Exception : public std::runtime_error
{
	public:
		Error505Exception(int client_socket, Response *response, Client *client);
};

#endif