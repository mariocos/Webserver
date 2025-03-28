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
	response->addToResponse(request->get_httpversion() + " 200 OK\r\n");
	response->addToResponse("Content-Type: " + response->getType() + "\r\n");
	if (client->getClientConnection() == true)
	{
		response->addToResponse("Connection: close\r\n");
		//response->addToResponse("Keep-Alive: timeout=5, max=100\r\n");
	}
	//if (response->getType() != "image/png" && response->getResponseLenght() > 5)
	//	response->addToResponse("Transfer-Enconding: chunked\r\n\r\n");
	//else
	//	response->addToResponse("Content-lenght: " + response->getResponseLenghtAsString() + "\r\n\r\n");
	response->addToResponse("Content-lenght: " + response->getResponseLenghtAsString() + "\r\n\r\n");
	send(client->getClientSocket(), response->getResponse().c_str(), response->getResponse().length(), MSG_NOSIGNAL);
	std::cout<<"response head:\n"<<response->getResponse();
	response->setResponse("");
}

void	loadImgResponse(int client_socket, Response *response, Client *client)
{
	std::ifstream	input;
	std::string		buffer;

	input.open(response->getPath().c_str());
	if (input.is_open())
	{
		while (getline(input, buffer))
			response->setResponse(response->getResponse().append(buffer + "\n"));
	}
	else
		throw Error404Exception(client_socket, response, client);
	input.close();
	if (send(client_socket, response->getResponse().c_str(), response->getResponse().length(), MSG_NOSIGNAL) == -1)
	{
		std::cout<<RED<<"Error sending the image"<<RESET<<std::endl;
		response->setResponse("");
		client->setClientWritingFlag(true);
		client->setClientPending(false);
		return ;
	}
	//response->addToBytesSent(send(client_socket, response->getResponse().c_str(), response->getResponse().length(), MSG_NOSIGNAL));
	//std::cout<<"Bytes that was supposed to send: "<<response->getResponseLenght()<<std::endl;
	//std::cout<<"Bytes sent: "<<response->getBytesSent()<<std::endl;
	response->setResponse("");
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

int setNonBlocking(int fd)
{
    return (fcntl(fd, F_SETFL, O_NONBLOCK));
}

void	loadPage(int client_socket, int input, Response *response, Client *client)
{
	int	msgsize = 0;
	char	buffer[7];
	ssize_t	bytes_read = -1;

	bzero(buffer, sizeof(buffer));
	while (bytes_read == -1 && msgsize < 5)
	{
		bytes_read = read(input, buffer, sizeof(buffer) - 1);
		if (bytes_read == -1)
		{
			perror("read: ");
			close(input);
		}
		if (bytes_read == 0)
			break ;
		msgsize += bytes_read;
		response->setResponse(buffer);
	}
	if (msgsize >= 5)
	{
		std::cout<<RED<<"Sent chunk"<<RESET<<std::endl;
		//std::cout<<"response body:\n"<<response->getResponse();
		//response->addToBytesSent(send(client_socket, response->getResponse().c_str(), response->getResponse().length(), MSG_NOSIGNAL));
		if (send(client_socket, response->getResponse().c_str(), response->getResponse().length(), MSG_NOSIGNAL) == -1)
		{
			std::cout<<RED<<"Error sending the chunk"<<RESET<<std::endl;
			response->setResponse("");
			close(input);
			client->setClientWritingFlag(true);
			client->setClientPending(false);
			return ;
		}
		if (client->getClientOpenFd() == -1)
			client->setClientOpenFd(input);
		client->setClientWritingFlag(false);
		client->setClientPending(true);
		return ;
	}
	if (send(client_socket, response->getResponse().c_str(), response->getResponse().length(), MSG_NOSIGNAL) == -1)
	{
		std::cout<<RED<<"Error sending the msg"<<RESET<<std::endl;
		response->setResponse("");
		close(input);
		client->setClientWritingFlag(true);
		client->setClientPending(false);
		return ;
	}
	//response->addToBytesSent(send(client_socket, response->getResponse().c_str(), response->getResponse().length(), MSG_NOSIGNAL));
	std::cout<<RED<<"Sent all the info"<<RESET<<std::endl;
	//std::cout<<"response body:\n"<<response->getResponse();
	//std::cout<<"Bytes that was supposed to send: "<<response->getResponseLenght()<<std::endl;
	//std::cout<<"Bytes sent: "<<response->getBytesSent()<<std::endl;
	response->setResponse("");
	close(input);
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}
