#include "includes/webserv.hpp"

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

	response.append("HTTP/1.1 503 Service Unavailable\r\n");
	response.append("Server: WebServ\r\n");
	response.append("Content-Type: text/plain\r\n");
	response.append("Content-Lenght: 0\r\n");
	response.append("Retry-After: 5\r\n\r\n");
	send(error_socket, response.c_str(), response.length(), MSG_NOSIGNAL);
	close(error_socket);
}

void	loadError505()
{
	
}
