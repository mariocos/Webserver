#include "includes/Errors.hpp"

Error400Exception::Error400Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 400 found") 
{
	loadError400(client_socket, response, client);
}

Error403Exception::Error403Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 403 found") 
{
	loadError403(client_socket, response, client);
}

Error404Exception::Error404Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 404 found") 
{
	loadError404(client_socket, response, client);
}

Error405Exception::Error405Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 405 found") 
{
	loadError405(client_socket, response, client);
}

Error413Exception::Error413Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 413 found") 
{
	loadError413(client_socket, response, client);
}

Error503Exception::Error503Exception(Client *errorClient, Server &server) :
runtime_error("Error 503 found") 
{
	loadError503(errorClient->getSocketFd());
	server.removeFromEpoll(errorClient->getSocketFd());
	delete	errorClient;
}

Error505Exception::Error505Exception() :
runtime_error("Error 505 found") 
{
	loadError505();
}
