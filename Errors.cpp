#include "includes/Errors.hpp"

Error400Exception::Error400Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 400 found") 
{
	response->setStatusCode(400);
	printLog("ERROR", NULL, client, response, 400);
	loadError400(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error403Exception::Error403Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 403 found") 
{
	response->setStatusCode(403);
	printLog("ERROR", NULL, client, response, 403);
	loadError403(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error404Exception::Error404Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 404 found") 
{
	response->setStatusCode(404);
	printLog("ERROR", NULL, client, response, 404);
	loadError404(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error405Exception::Error405Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 405 found") 
{
	response->setStatusCode(405);
	printLog("ERROR", NULL, client, response, 405);
	loadError405(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error409Exception::Error409Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 409 found")
{
	response->setStatusCode(409);
	printLog("ERROR", NULL, client, response, 409);
	loadError409(client_socket, response, client);
}

Error413Exception::Error413Exception(int client_socket, Response *response, Client *client) :
runtime_error("Error 413 found") 
{
	response->setStatusCode(413);
	printLog("ERROR", NULL, client, response, 413);
	loadError413(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error503Exception::Error503Exception(Client *errorClient, Server &server) :
runtime_error("Error 503 found") 
{
	printLog("ERROR", NULL, errorClient, errorClient->getClientResponse(), 503);
	errorClient->getClientResponse()->setStatusCode(503);
	loadError503(errorClient->getSocketFd());
	server.removeFromEpoll(errorClient->getSocketFd());
	delete	errorClient;
}

Error505Exception::Error505Exception() :
runtime_error("Error 505 found") 
{
	loadError505();
}
