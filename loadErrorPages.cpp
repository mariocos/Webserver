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
	std::string	path;

	response->clearResponse();
	response->addToResponse("HTTP/1.1 400 Bad Request\r\n");
	response->addToResponse("Content-Type: text/plain\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n\r\n");
	else
		response->addToResponse("Connection: close\r\n\r\n");
	if (client->getServerBlockTriggered()->getErrorPage(400) != client->getServerBlockTriggered()->getErrorMap().end())
		path = client->getServerBlockTriggered()->getErrorPage(400)->second;
	else
		path = "website/400.html";
	input.open(path.c_str(), std::ios::in | std::ios::binary);
	if (input.is_open())
	{
		std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
		std::string	type = getFileType(path);
		if (responseCopy.find("Content-Type: text/plain") != std::string::npos)
			responseCopy.replace(responseCopy.find("Content-Type: text/plain"), 24, "Content-Type: " + type);
		while (getline(input, buffer))
			responseCopy.append(buffer + "\n");
		input.close();
		send(client_socket, responseCopy.c_str(), responseCopy.size(), MSG_NOSIGNAL);
		response->clearResponse();
	}
	else
	{
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
	std::string	path;

	response->clearResponse();
	response->addToResponse("HTTP/1.1 403 Forbidden\r\n");
	response->addToResponse("Content-Type: text/plain\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n\r\n");
	else
		response->addToResponse("Connection: close\r\n\r\n");
	if (client->getServerBlockTriggered()->getErrorPage(403) != client->getServerBlockTriggered()->getErrorMap().end())
		path = client->getServerBlockTriggered()->getErrorPage(403)->second;
	else
		path = "website/403.html";
	input.open(path.c_str(), std::ios::in | std::ios::binary);
	if (input.is_open())
	{
		std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
		std::string	type = getFileType(path);
		if (responseCopy.find("Content-Type: text/plain") != std::string::npos)
			responseCopy.replace(responseCopy.find("Content-Type: text/plain"), 24, "Content-Type: " + type);
		while (getline(input, buffer))
			responseCopy.append(buffer + "\n");
		input.close();
		send(client_socket, responseCopy.c_str(), responseCopy.size(), MSG_NOSIGNAL);
		response->clearResponse();
	}
	else
	{
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
	std::string	path;

	response->clearResponse();
	response->addToResponse("HTTP/1.1 404 Not Found\r\n");
	response->addToResponse("Content-Type: text/plain\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n\r\n");
	else
		response->addToResponse("Connection: close\r\n\r\n");
	if (client->getServerBlockTriggered()->getErrorPage(404) != client->getServerBlockTriggered()->getErrorMap().end())
		path = client->getServerBlockTriggered()->getErrorPage(404)->second;
	else
		path = "website/404.html";
	input.open(path.c_str(), std::ios::in | std::ios::binary);
	if (input.is_open())
	{
		std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
		std::string	type = getFileType(path);
		if (responseCopy.find("Content-Type: text/plain") != std::string::npos)
			responseCopy.replace(responseCopy.find("Content-Type: text/plain"), 24, "Content-Type: " + type);
		while (getline(input, buffer))
			responseCopy.append(buffer + "\n");
		input.close();
		send(client_socket, responseCopy.c_str(), responseCopy.size(), MSG_NOSIGNAL);
		response->clearResponse();
	}
	else
	{
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
	std::string	path;

	response->clearResponse();
	response->addToResponse("HTTP/1.1 405 Method Not Allowed\r\n");
	response->addToResponse("Content-Type: text/plain\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n\r\n");
	else
		response->addToResponse("Connection: close\r\n\r\n");
	if (client->getServerBlockTriggered()->getErrorPage(405) != client->getServerBlockTriggered()->getErrorMap().end())
		path = client->getServerBlockTriggered()->getErrorPage(405)->second;
	else
		path = "website/405.html";
	input.open(path.c_str(), std::ios::in | std::ios::binary);
	if (input.is_open())
	{
		std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
		std::string	type = getFileType(path);
		if (responseCopy.find("Content-Type: text/plain") != std::string::npos)
			responseCopy.replace(responseCopy.find("Content-Type: text/plain"), 24, "Content-Type: " + type);
		while (getline(input, buffer))
			responseCopy.append(buffer + "\n");
		input.close();
		send(client_socket, responseCopy.c_str(), responseCopy.size(), MSG_NOSIGNAL);
		response->clearResponse();
	}
	else
	{
		response->addToResponse("405 Method Not Allowed\n");
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
	}
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError408(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;

	response->clearResponse();
	response->addToResponse("HTTP/1.1 408 Request Timeout\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n");
	else
		response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Type: text/plain\r\n");
	response->addToResponse("Content-Length: X\r\n\r\n");
	if (client->getServerBlockTriggered()->getErrorPage(408) != client->getServerBlockTriggered()->getErrorMap().end())
	{
		input.open(client->getServerBlockTriggered()->getErrorPage(408)->second.c_str(), std::ios::in | std::ios::binary);
		if (input.is_open())
		{
			std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
			std::string	type = getFileType(client->getServerBlockTriggered()->getErrorPage(408)->second);
			if (responseCopy.find("Content-Type: text/plain") != std::string::npos)
				responseCopy.replace(responseCopy.find("Content-Type: text/plain"), 24, "Content-Type: " + type);
			while (getline(input, buffer))
				responseCopy.append(buffer + "\n");
			input.close();
			send(client_socket, responseCopy.c_str(), responseCopy.size(), MSG_NOSIGNAL);
			response->clearResponse();
			client->setClientWritingFlag(true);
			client->setClientPending(false);
			return ;
		}
	}
	std::vector<uint8_t> data = response->getResponse();
	ssize_t	bytesSent = send(client_socket, reinterpret_cast<const char*>(data.data()), data.size(), MSG_NOSIGNAL);
	if (bytesSent == -1)
	{
		printLog("ERROR", NULL, client, NULL, 15, "");
		return ;
	}
	response->addToBytesSent(bytesSent);
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError409(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;

	response->clearResponse();
	response->addToResponse(client->getClientRequest()->get_httpversion() + " 409 Conflict\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n");
	else
		response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Type: text/plain\r\n\r\n");
	if (client->getServerBlockTriggered()->getErrorPage(409) != client->getServerBlockTriggered()->getErrorMap().end())
	{
		input.open(client->getServerBlockTriggered()->getErrorPage(409)->second.c_str(), std::ios::in | std::ios::binary);
		if (input.is_open())
		{
			std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
			std::string	type = getFileType(client->getServerBlockTriggered()->getErrorPage(409)->second);
			if (responseCopy.find("Content-Type: text/plain") != std::string::npos)
				responseCopy.replace(responseCopy.find("Content-Type: text/plain"), 24, "Content-Type: " + type);
			while (getline(input, buffer))
				responseCopy.append(buffer + "\n");
			input.close();
			send(client_socket, responseCopy.c_str(), responseCopy.size(), MSG_NOSIGNAL);
		}
		else
		{
			response->addToResponse("409 Conflict - Conflict with the file requested\n");
			sendMsgToSocket(client_socket, response->getResponse().size(), client, response);
		}
	}
	else
	{
		response->addToResponse("409 Conflict - Conflict with the file requested\n");
		sendMsgToSocket(client_socket, response->getResponse().size(), client, response);
	}
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError413(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;
	std::string	path;

	response->addToResponse("HTTP/1.1 413 Payload Too Large\r\n");
	response->addToResponse("Content-Type: text/plain\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n\r\n");
	else
		response->addToResponse("Connection: close\r\n\r\n");
	if (client->getServerBlockTriggered()->getErrorPage(413) != client->getServerBlockTriggered()->getErrorMap().end())
		path = client->getServerBlockTriggered()->getErrorPage(413)->second;
	else
		path = "website/413.html";
	input.open(path.c_str(), std::ios::in | std::ios::binary);
	if (input.is_open())
	{
		std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
		std::string	type = getFileType(path);
		if (responseCopy.find("Content-Type: text/plain") != std::string::npos)
			responseCopy.replace(responseCopy.find("Content-Type: text/plain"), 24, "Content-Type: " + type);
		while (getline(input, buffer))
			responseCopy.append(buffer + "\n");
		input.close();
		send(client_socket, responseCopy.c_str(), responseCopy.size(), MSG_NOSIGNAL);
		response->clearResponse();
	}
	else
	{
		response->addToResponse("413 Payload Too Large - The file is too big to handle\n");
		send(client_socket, &response->getResponse()[0], response->getResponse().size(), MSG_NOSIGNAL);
	}
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError417(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;
	std::string	body = "Expectation Failed.";

	response->clearResponse();
	response->addToResponse("HTTP/1.1 417 Expectation Failed\r\n");
	response->addToResponse("Content-Type: text/plain\r\n");
	response->addToResponse("Content-Length: " + transformToString(body.length()) + "\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n\r\n");
	else
		response->addToResponse("Connection: close\r\n\r\n");
	if (client->getServerBlockTriggered()->getErrorPage(417) != client->getServerBlockTriggered()->getErrorMap().end())
	{
		input.open(client->getServerBlockTriggered()->getErrorPage(417)->second.c_str(), std::ios::in | std::ios::binary);
		if (input.is_open())
		{
			std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
			std::string	type = getFileType(client->getServerBlockTriggered()->getErrorPage(417)->second);
			if (responseCopy.find("Content-Type: text/plain") != std::string::npos)
				responseCopy.replace(responseCopy.find("Content-Type: text/plain"), 24, "Content-Type: " + type);
			while (getline(input, buffer))
				responseCopy.append(buffer + "\n");
			input.close();
			send(client_socket, responseCopy.c_str(), responseCopy.size(), MSG_NOSIGNAL);
			response->clearResponse();
			client->setClientWritingFlag(true);
			client->setClientPending(false);
			return ;
		}
	}
	response->addToResponse(body);
	sendMsgToSocket(client_socket, response->getResponse().size(), client, response);
	response->clearResponse();
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

void	loadError505(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;
	std::string	body = "HTTP Version Not Supported.";

	response->clearResponse();
	response->addToResponse("HTTP/1.1 505 HTTP Version Not Supported\r\n");
	response->addToResponse("Content-Type: text/plain\r\n");
	response->addToResponse("Content-Length: " + transformToString(body.length()) + "\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n\r\n");
	else
		response->addToResponse("Connection: close\r\n\r\n");
	if (client->getServerBlockTriggered()->getErrorPage(505) != client->getServerBlockTriggered()->getErrorMap().end())
	{
		input.open(client->getServerBlockTriggered()->getErrorPage(505)->second.c_str(), std::ios::in | std::ios::binary);
		if (input.is_open())
		{
			std::string	responseCopy(response->getResponse().begin(), response->getResponse().end());
			std::string	type = getFileType(client->getServerBlockTriggered()->getErrorPage(505)->second);
			if (responseCopy.find("Content-Type: text/plain") != std::string::npos)
				responseCopy.replace(responseCopy.find("Content-Type: text/plain"), 24, "Content-Type: " + type);
			while (getline(input, buffer))
				responseCopy.append(buffer + "\n");
			input.close();
			send(client_socket, responseCopy.c_str(), responseCopy.size(), MSG_NOSIGNAL);
			response->clearResponse();
			client->setClientWritingFlag(true);
			client->setClientPending(false);
			return ;
		}
	}
	response->addToResponse(body);
	sendMsgToSocket(client_socket, response->getResponse().size(), client, response);
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}
