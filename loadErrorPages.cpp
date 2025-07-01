#include "includes/webserv.hpp"

void	load301(int client_socket, Response *response, Client *client)
{
	response->clearResponse();
	response->addToResponse("HTTP/1.1 301 Moved Permanently\r\n");
	response->addToResponse("Location: " + client->getRouteTriggered()->getRedirectPath() + "\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n");
	else
		response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Length: 0\r\n\r\n");
	sendMsgToSocket(client_socket, response->getResponse().size(), client, response);
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	load307(int client_socket, Response *response, Client *client)
{
	response->clearResponse();
	response->addToResponse("HTTP/1.1 307 Temporary Redirect\r\n");
	response->addToResponse("Location: " + client->getRouteTriggered()->getRedirectPath() + "\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n");
	else
		response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Length: 0\r\n\r\n");
	sendMsgToSocket(client_socket, response->getResponse().size(), client, response);
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError400(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;

	response->clearResponse();
	response->addToResponse("HTTP/1.1 400 Bad Request\r\n");
	response->addToResponse("Content-Type: text/html\r\n");
	response->addToResponse("Connection: close\r\n\r\n");
	input.open("website/400.html", std::ios::in | std::ios::binary);
	if (input.is_open())
	{
		std::string	responseCopy;
		while (getline(input, buffer))
			responseCopy.append(buffer + "\n");
		input.close();
		response->addToResponse(responseCopy);
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
		response->clearResponse();
	}
	else
	{
		std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
		if (responseCopy.find("Content-Type: text/html") != std::string::npos)
			responseCopy.replace(responseCopy.find("Content-Type: text/html"), 24, "Content-Type: text/plain");
		response->getResponse().assign(responseCopy.begin(), responseCopy.end());
		response->addToResponse("400 Bad Request - The server could not understand the request\n");
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
	}
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError403(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;

	response->clearResponse();
	response->addToResponse("HTTP/1.1 403 Forbidden\r\n");
	response->addToResponse("Content-Type: text/html\r\n");
	response->addToResponse("Connection: close\r\n\r\n");
	input.open("website/403.html", std::ios::in | std::ios::binary);
	if (input.is_open())
	{
		std::string	responseCopy;
		while (getline(input, buffer))
			responseCopy.append(buffer + "\n");
		input.close();
		response->addToResponse(responseCopy);
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
		response->clearResponse();
	}
	else
	{
		std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
		if (responseCopy.find("Content-Type: text/html") != std::string::npos)
			responseCopy.replace(responseCopy.find("Content-Type: text/html"), 24, "Content-Type: text/plain");
		response->getResponse().assign(responseCopy.begin(), responseCopy.end());
		response->addToResponse("403 Forbidden - You do not have the right permissions to access this file\n");
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
	}
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError404(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;

	response->clearResponse();
	response->addToResponse("HTTP/1.1 404 Not Found\r\n");
	response->addToResponse("Content-Type: text/html\r\n");
	response->addToResponse("Connection: close\r\n\r\n");
	input.open("website/404.html", std::ios::in | std::ios::binary);
	if (input.is_open())
	{
		std::string	responseCopy;
		while (getline(input, buffer))
			responseCopy.append(buffer + "\n");
		input.close();
		response->addToResponse(responseCopy);
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
		response->clearResponse();
	}
	else
	{
		std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
		if (responseCopy.find("Content-Type: text/html") != std::string::npos)
			responseCopy.replace(responseCopy.find("Content-Type: text/html"), 24, "Content-Type: text/plain");
		response->getResponse().assign(responseCopy.begin(), responseCopy.end());
		response->addToResponse("404 Not Found - The server could not find the file requested\n");
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
	}
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError405(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;

	response->clearResponse();
	response->addToResponse("HTTP/1.1 405 Method Not Allowed\r\n");
	response->addToResponse("Content-Type: text/html\r\n");
	response->addToResponse("Connection: close\r\n\r\n");
	input.open("website/405.html", std::ios::in | std::ios::binary);
	if (input.is_open())
	{
		std::string	responseCopy;
		while (getline(input, buffer))
			responseCopy.append(buffer + "\n");
		input.close();
		response->addToResponse(responseCopy);
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
		response->clearResponse();
	}
	else
	{
		std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
		if (responseCopy.find("Content-Type: text/html") != std::string::npos)
			responseCopy.replace(responseCopy.find("Content-Type: text/html"), 24, "Content-Type: text/plain");
		response->getResponse().assign(responseCopy.begin(), responseCopy.end());
		response->addToResponse("405 Method Not Allowed\n");
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
	}
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError408(int client_socket, Response *response, Client *client)
{
	response->clearResponse();
	response->addToResponse("HTTP/1.1 408 Request Timeout\r\n");
	response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Length: X\r\n\r\n");
	sendMsgToSocket(client_socket, response->getResponse().size(), client, response);
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError409(int client_socket, Response *response, Client *client)
{
	response->clearResponse();
	response->addToResponse(client->getClientRequest()->get_httpversion() + " 409 Conflict\r\n");
	response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Length: 0\r\n\r\n");
	sendMsgToSocket(client_socket, response->getResponse().size(), client, response);
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError413(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;

	response->addToResponse("HTTP/1.1 413 Payload Too Large\r\n");
	response->addToResponse("Content-Type: text/html\r\n");
	response->addToResponse("Connection: close\r\n\r\n");
	input.open("website/413.html", std::ios::in | std::ios::binary);
	if (input.is_open())
	{
		std::string	responseCopy;
		while (getline(input, buffer))
			responseCopy.append(buffer + "\n");
		input.close();
		response->addToResponse(responseCopy);
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
		response->clearResponse();
	}
	else
	{
		std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
		if (responseCopy.find("Content-Type: text/html") != std::string::npos)
			responseCopy.replace(responseCopy.find("Content-Type: text/html"), 24, "Content-Type: text/plain");
		response->getResponse().assign(responseCopy.begin(), responseCopy.end());
		response->addToResponse("413 Payload Too Large - The file is too big to handle\n");
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
	}
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError503(int error_socket)
{
	std::string response;
	std::string	body = "Webserv is busy. Try again later.";
	ssize_t	bytesSent = 0;
	ssize_t bytesToSend;

	response.append("HTTP/1.1 503 Service Unavailable\r\n");
	response.append("Server: WebServ\r\n");
	response.append("Content-Type: text/plain\r\n");
	response.append("Content-Length: " + transformToString(body.length()) + "\r\n");
	response.append("Connection: close\r\n\r\n");
	response.append(body);
	bytesToSend = response.length();
	const char	*str = response.c_str();
	while (bytesSent < bytesToSend)
	{
		ssize_t	bytes = send(error_socket, str + bytesSent, bytesToSend - bytesSent, MSG_NOSIGNAL);
		if (bytes <= 0)
			break;
		bytesSent += bytes;
	}
	close(error_socket);
}

void	loadError505()
{
}
