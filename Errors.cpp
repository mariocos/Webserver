#include "includes/Errors.hpp"

Load301Exception::Load301Exception(int client_socket, Response *response, Client *client) :
runtime_error("Redirect") 
{
	response->setStatusCode(301);
	printLog("INFO", NULL, client, NULL, 301);
	load301(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Load307Exception::Load307Exception(int client_socket, Response *response, Client *client) :
runtime_error("Redirect") 
{
	response->setStatusCode(307);
	printLog("INFO", NULL, client, NULL, 307);
	load307(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error400Exception::Error400Exception(int client_socket, Response *response, Client *client) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] Bad Request 400 from " + client->getClientIp() + ":" + transformToString(client->getClientPort()) + RESET) 
{
	response->setStatusCode(400);
	loadError400(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error403Exception::Error403Exception(int client_socket, Response *response, Client *client) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] Forbidden 403 - " + client->getClientRequest()->get_path() + RESET) 
{
	response->setStatusCode(403);
	loadError403(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error404Exception::Error404Exception(int client_socket, Response *response, Client *client) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] File Not Found 404 - " + client->getClientRequest()->get_path() + RESET) 
{
	response->setStatusCode(404);
	loadError404(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error405Exception::Error405Exception(int client_socket, Response *response, Client *client) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] Method Not Allowed 405 - " + client->getClientRequest()->get_method() + RESET) 
{
	response->setStatusCode(405);
	loadError405(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error409Exception::Error409Exception(int client_socket, Response *response, Client *client) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] Conflit 409 - " + client->getClientRequest()->get_path() + RESET)
{
	response->setStatusCode(409);
	loadError409(client_socket, response, client);
}

Error413Exception::Error413Exception(int client_socket, Response *response, Client *client) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] Payload Too Large 413 " + client->getClientRequest()->get_path() + RESET) 
{
	response->setStatusCode(413);
	loadError413(client_socket, response, client);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}

Error503Exception::Error503Exception(Client *errorClient, Server &server) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] " + errorClient->getClientIp() + ":" + transformToString(errorClient->getClientPort()) + " - 503 Webserver Busy" + RESET)
{
	errorClient->getClientResponse()->setStatusCode(503);
	loadError503(errorClient->getSocketFd());
	server.removeFromEpoll(errorClient->getSocketFd());
	delete	errorClient;
}
