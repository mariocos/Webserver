#include "webserv.hpp"

void	findType(RequestParse *request, Response *response)
{
	if (request->get_path() == "/")
		request->set_path("/dummy.html");
	if (request->get_path().length() > 5 && request->get_path().find(".html") == request->get_path().length() - 5)
		response->setType("text/html");
	else if (request->get_path().length() > 4 && request->get_path().find(".css") == request->get_path().length() - 4)
		response->setType("text/css");
	else if (request->get_path().length() > 4 && request->get_path().find(".png") == request->get_path().length() - 4)
		response->setType("image/png");
	else
		response->setType("text/html");
	response->setPath("website" + request->get_path());
}

void	createHeader(RequestParse *request, Response *response, Client *client)
{
	response->addToResponse(request->get_httpversion() + "200 OK\n");
	response->addToResponse("Content-Type: " + response->getType() + "\n");
	if (response->getType() != "image/png")
		response->addToResponse("Transfer-Enconding: chunked\n\n");
	else
		response->addToResponse("\n");
	send(client->getClientSocket(), response->getResponse().c_str(), response->getResponse().length(), MSG_DONTWAIT);
	response->setResponse("");
}

bool	loadImgResponse(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string		buffer;
	ssize_t	msgsize = 0;
	std::string	lenght;
	std::ostringstream	number;

	input.open(response->getPath().c_str());
	if (input.is_open())
	{
		while (getline(input, buffer))
		{
			response->setResponse(response->getResponse().append(buffer + "\n"));
			msgsize += buffer.length();
		}
	}
	else
	{
		input.open("website/404.html");
		while (getline(input, buffer))
		{
			response->setResponse(response->getResponse().append(buffer + "\n"));
			msgsize += buffer.length();
		}
	}
	input.close();
	number<<msgsize;
	lenght = number.str();
	if (response->getResponse().find("Transfer-Enconding: chunked") != std::string::npos)
		response->setResponse(response->getResponse().replace(response->getResponse().find("Transfer-Enconding: chunked"), 28, "Content-lenght: " + lenght + "\n"));
	send(client_socket, response->getResponse().c_str(), response->getResponse().length(), MSG_DONTWAIT);
	(void) client;
	//if (client->getClientConnection() == false)
		close(client_socket);
	return (false);
}

bool	loadErrorPage(int client_socket, Response *response, Client *client)
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
			response->setResponse(response->getResponse().replace(response->getResponse().find("Transfer-Enconding: chunked"), 28, "Content-lenght: " + lenght + "\n"));
		send(client_socket, response->getResponse().c_str(), response->getResponse().length(), MSG_DONTWAIT);
	}
	else
		std::cout<<"Error oppening the file"<<std::endl;
	(void) client;
	//if (client->getClientConnection() == false)
		close(client_socket);
	return (false);
}

int setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return (-1); // Error
    }
    return (fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}

bool	loadPage(int client_socket, int input, Response *response, Client *client)
{
	int	msgsize = 0;
	char	buffer[1024];
	ssize_t	bytes_read = -1;
	std::string	lenght;
	std::ostringstream	number;

	bzero(buffer, sizeof(buffer));
	while (bytes_read == -1)
	{
		bytes_read = read(input, buffer, sizeof(buffer) - 1);
		if (bytes_read == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				continue;
			else
			{
				perror("read: ");
				close(input);
			}
		}
		if (bytes_read == 0)
			break ;
		msgsize += bytes_read;
		if (client->getClientPending() == false)
			response->setResponse(response->getResponse().append(buffer));
		else
			response->setResponse(buffer);
	}
	if (msgsize >= 1022)
	{
		send(client_socket, response->getResponse().c_str(), response->getResponse().length(), MSG_DONTWAIT);
		std::cout<<"response: "<<response->getResponse()<<std::endl;
		if (client->getClientOpenFd() == -1)
			client->setClientOpenFd(input);
		return (true);
	}
	number<<msgsize;
	lenght = number.str();
	if (response->getResponse().find("Transfer-Enconding: chunked") != std::string::npos)
		response->setResponse(response->getResponse().replace(response->getResponse().find("Transfer-Enconding: chunked"), 28, "Content-lenght: " + lenght + "\n"));
	send(client_socket, response->getResponse().c_str(), response->getResponse().length(), MSG_DONTWAIT);
	std::cout<<"response: "<<response->getResponse()<<std::endl;
	//if (client->getClientConnection() == false)
		close(client_socket);
	close(input);
	return (false);
}
