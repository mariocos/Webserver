#include "webserv.hpp"

void	loadError400(int client_socket, Client *client)
{
	std::string response;
	response.append("HTTP/1.1 400 Bad Request\r\n");
	response.append("Content-Type: text/plain\r\n");
	response.append("Connection: close\r\n\r\n");
	response.append("400 Bad Request - The server could not understand the request\n");
	send(client_socket, response.c_str(), response.length(), O_NONBLOCK);
	client->setClientWritingFlag(true);
	close(client_socket);
}

void	loadError403()
{

}

void	loadError404(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string	buffer;
	ssize_t	msgsize = 0;
	std::string	lenght;
	std::ostringstream	number;

	input.open("website/404.html");
	if (input.is_open())
	{
		while (getline(input, buffer))
		{
			response->setResponse(response->getResponse().append(buffer + "\n"));
			msgsize += buffer.length();
		}
		input.close();
		number<<msgsize;
		lenght = number.str();
		if (response->getResponse().find("Transfer-Enconding: chunked") != std::string::npos)
			response->setResponse(response->getResponse().replace(response->getResponse().find("Transfer-Enconding: chunked"), 28, "Content-lenght: " + lenght));
		send(client_socket, response->getResponse().c_str(), response->getResponse().length(), O_NONBLOCK);
		response->setResponse("");
	}
	else
		std::cout<<"Error oppening the file"<<std::endl;
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	loadError405()
{
	
}

void	loadError503(int error_socket)
{
	std::string response;
	response.append("HTTP/1.1 503 Service Unavailable\r\n");
	response.append("Date: Wed, 06 Mar 2024 12:00:00 GMT\r\n");
	response.append("Server: WebServ\r\n");
	response.append("Content-Type: text\r\n");
	response.append("Content-Lenght: 0\r\n");
	response.append("Retry-After: 10\r\n\r\n");
	send(error_socket, response.c_str(), response.length(), O_NONBLOCK);
	close(error_socket);
}

void	loadError505()
{
	
}
