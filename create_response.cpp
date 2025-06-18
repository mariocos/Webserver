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
	//for some reason with this type when requesting a directory in the browser
	//it presents a menu like for downloading a file when requesting for a unknown type of file
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

	//TODO
	//To be added after getting the client_max_body_size
	/* if (client->getClientFile()->getFileStats()->st_size > max_body_size)
		throw Error413Exception(client->getSocketFd(), response, client); */

	response->addToResponseLenght(client->getClientFile()->getFileStats()->st_size);
	response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Length: " + response->getResponseLenghtAsString() + "\r\n\r\n");
	response->addToBytesToSend(response->getResponse().size());
	sendMsgToSocket(client->getSocketFd(), response->getResponse().size(), client, response);
	//std::cout<<"response head:\n"<<std::string(response->getResponse().begin(), response->getResponse().end());
	response->clearResponse();
	client->getClientFile()->setReading(true);
	client->getClientFile()->setWriting(false);
	response->addToBytesToSend(client->getClientFile()->getFileStats()->st_size);
	printLog("ACCESS", client->getServerBlockTriggered(), client, client->getClientResponse(), 5);
}

int setNonBlocking(int fd)
{
    return (fcntl(fd, F_SETFL, O_NONBLOCK));
}

void	loadPage(int client_socket, Response *response, Client *client)
{
	response->setResponse(client->getClientFile()->readFromBuffer());
	client->getClientFile()->clearBuffer();
	client->getClientFile()->setReading(true);
	client->getClientFile()->setWriting(false);
	if (response->getBytesSent() < client->getClientFile()->getFileStats()->st_size)
		sendMsgToSocket(client_socket, client->getClientFile()->getBytesRead(), client, response);
	if (response->getBytesSent() < client->getClientFile()->getFileStats()->st_size)
	{
		printLog("DEBUG", client->getServerBlockTriggered(), client, response, 11);
		client->setClientWritingFlag(false);
		client->setClientPending(true);
		return ;
	}
	if (response->getBytesSent() == response->getBytesToSend() && !client->getClientWritingFlag())
		printLog("INFO", client->getServerBlockTriggered(), client, response, 6);
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
}

void	createPostResponse(Client *client, Response *response)
{
	response->clearResponse();
	response->setStatusCode(201);
	response->addToResponse(client->getClientRequest()->get_httpversion() + " 201 Created\r\n");
	response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Length: 20\r\n\r\n");
	response->addToResponse("Created successfully");
	sendMsgToSocket(client->getSocketFd(), response->getResponse().size(), client, response);
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
	printLog("INFO", client->getServerBlockTriggered(), client, response, 9);
}

void	createDeleteResponse(Client *client, Response *response)
{
	response->clearResponse();
	response->setStatusCode(200);
	response->addToResponse(client->getClientRequest()->get_httpversion() + " 200 OK\r\n");
	response->addToResponse("Content-Type: text/plain");
	response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Length: 20\r\n\r\n");
	response->addToResponse("Deleted successfully");
	sendMsgToSocket(client->getSocketFd(), response->getResponse().size(), client, response);
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
	printLog("INFO", client->getServerBlockTriggered(), client, response, 10);
}

void	sendMsgToSocket(int client_socket, int lenght, Client *client, Response *response)
{
	std::vector<uint8_t> data = response->getResponse();
	ssize_t	bytesSent = send(client_socket, reinterpret_cast<const char*>(data.data()), lenght, MSG_NOSIGNAL);
	if (bytesSent == -1)
		throw SendException(client, response);
	response->addToBytesSent(bytesSent);
}
