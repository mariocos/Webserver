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
	if (client->getClientConnection() == true)
		response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Length: " + response->getResponseLenghtAsString() + "\r\n\r\n");
	sendMsgToSocket(client->getSocketFd(), response->getResponse().size(), client, response);
	std::cout<<"response head:\n"<<std::string(response->getResponse().begin(), response->getResponse().end());
	response->clearResponse();
	client->getClientFile()->setReading(true);
	client->getClientFile()->setWriting(false);
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
	response->addToBytesSent(client->getClientFile()->getBytesRead());
	sendMsgToSocket(client_socket, client->getClientFile()->getBytesRead(), client, response);
	if (response->getBytesSent() < client->getClientFile()->getFileStats()->st_size)
	{
		client->setClientWritingFlag(false);
		client->setClientPending(true);
		return ;
	}
	printLog("INFO", client->getServerBlockTriggered(), client, response, 6);
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
