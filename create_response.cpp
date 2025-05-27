#include "includes/webserv.hpp"

std::string	findFileExtension(std::string path)
{
	size_t	pos = path.rfind(".");
	if (pos != std::string::npos)
		return (path.substr(pos));
	return ("");
}

void	findType(RequestParse *request, Response *response)
{
	std::map<std::string, std::string> fileTypes;
	fileTypes[".html"] = "text/html";
	fileTypes[".css"] = "text/css";
	fileTypes[".js"] = "application/javascript";
	fileTypes[".png"] = "image/png";
	fileTypes[".jpg"] = "image/jpeg";
	fileTypes[".jpeg"] = "image/jpeg";
	fileTypes[".ico"] = "image/x-icon";
	fileTypes[".json"] = "application/json";
	fileTypes[".mp3"] = "audio/mpeg";
	fileTypes[".mp4"] = "video/mp4";
	fileTypes[".txt"] = "text/plain";

	if (request->get_path() == "/")
		request->set_path("/dummy.html");

	std::string	extension = findFileExtension(request->get_path());
	std::string	type = "application/octet-stream";

	if (fileTypes.count(extension))
		type = fileTypes.at(extension);

	response->setType(type);
	response->setPath("website" + request->get_path());
}

void	createHeader(RequestParse *request, Response *response, Client *client)
{
	response->clearResponse();
	response->setStatusCode(200);
	response->addToResponse(request->get_httpversion() + " 200 OK\r\n");
	response->addToResponse("Content-Type: " + response->getType() + "\r\n");

	//To be added after getting the client_max_body_size
	/* if (client->getClientFile()->getFileStats()->st_size > max_body_size)
		throw Error413Exception(client->getSocketFd(), response, client); */

	response->addToResponseLenght(client->getClientFile()->getFileStats()->st_size);
	if (client->getClientConnection() == true)
		response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Length: " + response->getResponseLenghtAsString() + "\r\n\r\n");
	sendMsgToSocket(client->getSocketFd(), response->getResponse().size(), client, response);
	std::cout<<"response head:\n"<<std::string(response->getResponse().begin(), response->getResponse().end());
	response->clearResponse();
	client->getClientFile()->setReading(true);
	client->getClientFile()->setWriting(false);
	printLog("ACCESS", client->getServerBlockTriggered(), client, client->getClientResponse(), 3);
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
	if (client->getClientFile()->getBytesRead() >= buffer_size)
	{
		//std::cout<<"response body:\n"<<response->getResponse();
		//response->addToBytesSent(send(client_socket, response->getResponse().c_str(), client->getClientFile()->getBytesRead(), MSG_NOSIGNAL));
		//std::cout<<"Bytes sent until now: "<<response->getBytesSent()<<std::endl;
		sendMsgToSocket(client_socket, buffer_size, client, response);
		client->setClientWritingFlag(false);
		client->setClientPending(true);
		return ;
	}
	sendMsgToSocket(client_socket, client->getClientFile()->getBytesRead(), client, response);
	printLog("INFO", client->getServerBlockTriggered(), client, response, 4);
	//response->addToBytesSent(send(client_socket, response->getResponse().c_str(), client->getClientFile()->getBytesRead(), MSG_NOSIGNAL));
	//std::cout<<"response body:\n"<<response->getResponse();
	//std::cout<<"Bytes that was supposed to send: "<<response->getResponseLenght()<<std::endl;
	//std::cout<<"Bytes sent: "<<response->getBytesSent()<<std::endl;
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	sendMsgToSocket(int client_socket, int lenght, Client *client, Response *response)
{
	std::vector<uint8_t> data = response->getResponse();
	if (send(client_socket, reinterpret_cast<const char*>(data.data()), lenght, MSG_NOSIGNAL) == -1)
		throw SendException(client, response);
}
