#include "includes/webserv.hpp"

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
	response->addToResponseLenght(client->getClientFile()->getFileStats()->st_size);
	if (client->getClientConnection() == true)
		response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-lenght: " + response->getResponseLenghtAsString() + "\r\n\r\n");
	sendMsgToSocket(client->getClientSocket(), response->getResponse().length(), client, response);
	std::cout<<"response head:\n"<<response->getResponse();
	response->setResponse("");
	client->getClientFile()->setReading(true);
	client->getClientFile()->setWriting(false);
}

int setNonBlocking(int fd)
{
    return (fcntl(fd, F_SETFL, O_NONBLOCK));
}

void	loadPage(int client_socket, unsigned int buffer_size, Response *response, Client *client)
{
	response->setResponse(client->getClientFile()->readFromBuffer());
	client->getClientFile()->clearBuffer();
	client->getClientFile()->setReading(true);
	client->getClientFile()->setWriting(false);
	if (client->getClientFile()->getBytesRead() >= buffer_size - 1)
	{
		std::cout<<RED<<"Sent chunk"<<RESET<<std::endl;
		//std::cout<<"response body:\n"<<response->getResponse();
		//response->addToBytesSent(send(client_socket, response->getResponse().c_str(), client->getClientFile()->getBytesRead(), MSG_NOSIGNAL));
		//std::cout<<"Bytes sent until now: "<<response->getBytesSent()<<std::endl;
		sendMsgToSocket(client_socket, buffer_size, client, response);
		client->setClientWritingFlag(false);
		client->setClientPending(true);
		return ;
	}
	sendMsgToSocket(client_socket, client->getClientResponse()->getResponseLenght(), client, response);
	//response->addToBytesSent(send(client_socket, response->getResponse().c_str(), client->getClientFile()->getBytesRead(), MSG_NOSIGNAL));
	std::cout<<RED<<"Sent all the info"<<RESET<<std::endl;
	//std::cout<<"response body:\n"<<response->getResponse();
	//std::cout<<"Bytes that was supposed to send: "<<response->getResponseLenght()<<std::endl;
	//std::cout<<"Bytes sent: "<<response->getBytesSent()<<std::endl;
	response->setResponse("");
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	sendMsgToSocket(int client_socket, int lenght, Client *client, Response *response)
{
	if (send(client_socket, response->getResponse().c_str(), lenght, MSG_NOSIGNAL) == -1)
		throw SendException(client, response);
}
