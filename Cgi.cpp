#include "includes/Cgi.hpp"

Cgi::Cgi() : _pid(-1), _running(false)
{
	this->_cgiStdIn[0] = -1;
	this->_cgiStdIn[1] = -1;
	this->_cgiStdOut[0] = -1;
	this->_cgiStdOut[1] = -1;
}

Cgi::~Cgi()
{
	if (this->_cgiStdIn[0] != -1)
		close(this->_cgiStdIn[0]);
	if (this->_cgiStdIn[1] != -1)
		close(this->_cgiStdIn[1]);
	if (this->_cgiStdOut[0] != -1)
		close(this->_cgiStdOut[0]);
	if (this->_cgiStdOut[1] != -1)
		close(this->_cgiStdOut[1]);
}

std::vector<std::string>	&Cgi::getEnv()
{
	return (this->_envp);
}

std::vector<uint8_t>	&Cgi::getCgiResponse()
{
	return (this->_cgiResponse);
}

pid_t	Cgi::getPid()
{
	return (this->_pid);
}

void	Cgi::setEnv(std::vector<std::string> env)
{
	this->_envp = env;
}

void	Cgi::setPid(pid_t pid)
{
	this->_pid = pid;
}

void	Cgi::setCgiResponse(unsigned int buffer_size, Response *response)
{
	std::vector<uint8_t>	binaryBuffer(buffer_size);
	ssize_t	bytesRead = 1;
	std::string	buffer;

	while (bytesRead > 0)
	{
		bytesRead = read(this->_cgiStdOut[0], reinterpret_cast<char*>(binaryBuffer.data()), buffer_size);
		if (bytesRead < 0)
			return ;
		else if (bytesRead == 0)
			break;
		buffer = transformToString(binaryBuffer.data());
		if (buffer.find("Content-Type:") != std::string::npos)
		{
			response->setType(buffer.substr(buffer.find(":") + 2, buffer.find_first_of("\n") - 14));
			ssize_t headerEnd = buffer.find(response->getType());
			headerEnd += response->getType().size() + 2;
			this->_cgiResponse.insert(this->_cgiResponse.end(), binaryBuffer.begin() + headerEnd, binaryBuffer.begin() + bytesRead);
		}
		else
			this->_cgiResponse.insert(this->_cgiResponse.end(), binaryBuffer.begin(), binaryBuffer.begin() + bytesRead);
	}
	response->addToBytesToSend(this->_cgiResponse.size());
}

void	Cgi::changeCgiState()
{
	if (this->_running)
		this->_running = false;
	else
		this->_running = true;
}

int	*Cgi::getStdIn()
{
	return (this->_cgiStdIn);
}

int	*Cgi::getStdOut()
{
	return (this->_cgiStdOut);
}

bool	Cgi::isActive()
{
	return (this->_running);
}

void	Cgi::executeCgi(Client *client)
{
	std::string	path = client->getNewPath();
	std::vector<char*>	env;
	std::string	scriptDir;
	std::string	scriptName;
	for (size_t i = 0; i < this->_envp.size(); i++)
	{
		env.push_back(const_cast<char*>(this->_envp[i].c_str()));
	}
	env.push_back(NULL);
	scriptDir = path.substr(0, path.find_last_of("/"));
	if (chdir(scriptDir.c_str()) == -1)
		throw BadChildException();
	scriptName = path.substr(path.find_last_of("/") + 1);
	if (dup2(this->_cgiStdIn[0], STDIN_FILENO) == -1)
		throw BadChildException();
	if (dup2(this->_cgiStdOut[1], STDOUT_FILENO) == -1)
		throw BadChildException();
	close(this->_cgiStdIn[1]);
	close(this->_cgiStdOut[0]);
	this->_cgiStdIn[1] = -1;
	this->_cgiStdOut[0] = -1;
	char	*av[] = {const_cast<char*>(scriptName.c_str()), NULL};
	execve(scriptName.c_str(), av, env.data());
	throw BadChildException();
}

void	Cgi::parentWork(Server &server, Client *client)
{
	struct epoll_event	event;
	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = this->_cgiStdOut[0];
	epoll_ctl(server.getEpollFd(), EPOLL_CTL_ADD, event.data.fd, &event);
	close(this->_cgiStdIn[0]);
	close(this->_cgiStdOut[1]);
	this->_cgiStdIn[0] = -1;
	this->_cgiStdOut[1] = -1;
	client->getClientFile()->setReading(true);
	client->getClientFile()->setWriting(false);
	if (client->getClientRequest()->get_method() == "POST")
	{
		std::string	body = client->getClientRequest()->get_content();
		ssize_t	bytesSent = 0;
		ssize_t	bodyLenght = body.size();
		while (bytesSent < bodyLenght)
		{
			ssize_t	n = write(this->_cgiStdIn[1], body.c_str() + bytesSent, bodyLenght - bytesSent);
			if (n <= 0)
				break;
			bytesSent += n;
		}
	}
	close(this->_cgiStdIn[1]);
	this->_cgiStdIn[1] = -1;
	this->changeCgiState();
	client->getClientResponse()->setStatusCode(200);
}

void	Cgi::readCgiResponse(Server &server, Client *client)
{
	this->setCgiResponse(1048576, client->getClientResponse());
	if (this->_cgiResponse.size() == 0)
		client->getClientResponse()->setStatusCode(403);
	printLog("CGI", NULL, client, client->getClientResponse(), 8);
	server.removeFromEpoll(this->_cgiStdOut[0]);
	close(this->_cgiStdOut[0]);
	this->_cgiStdOut[0] = -1;
	this->changeCgiState();
	client->getClientFile()->setReading(false);
	client->getClientFile()->setWriting(true);
}

void	Cgi::writeCgiResponse(Client *client)
{
	client->getClientResponse()->clearResponse();
	if (client->getClientResponse()->getStatusCode() == 200)
		client->getClientResponse()->addToResponse(client->getClientRequest()->get_httpversion() + " " + transformToString(client->getClientResponse()->getStatusCode()) + " OK\r\n");
	else
		client->getClientResponse()->addToResponse(client->getClientRequest()->get_httpversion() + " " + transformToString(client->getClientResponse()->getStatusCode()) + " KO\r\n");
	client->getClientResponse()->addToResponse("Content-Type: " + client->getClientResponse()->getType() + "\r\n");
	client->getClientResponse()->addToResponseLenght(this->_cgiResponse.size());
	client->getClientResponse()->addToResponse("Connection: close\r\n");
	client->getClientResponse()->addToResponse("Content-Length: " + client->getClientResponse()->getResponseLenghtAsString() + "\r\n\r\n");
	//std::cout<<"response head:\n"<<std::string(client->getClientResponse()->getResponse().begin(), client->getClientResponse()->getResponse().end());
	client->getClientResponse()->addToBytesToSend(client->getClientResponse()->getResponse().size());
	sendMsgToSocket(client->getSocketFd(), client->getClientResponse()->getResponse().size(), client, client->getClientResponse());
	client->getClientResponse()->clearResponse();
	std::vector<uint8_t> data = this->_cgiResponse;
	while (client->getClientResponse()->getBytesSent() < client->getClientResponse()->getBytesToSend())
	{
		ssize_t	bytesSent = send(client->getSocketFd(), reinterpret_cast<const char*>(data.data()), this->_cgiResponse.size(), MSG_NOSIGNAL);
		if (bytesSent < 0)
			throw SendException(client, client->getClientResponse());
		else if (bytesSent == 0)
			break;
		client->getClientResponse()->addToBytesSent(bytesSent);
	}
	client->getClientFile()->setReading(true);
	client->getClientFile()->setWriting(true);
	client->setClientReadingFlag(true);
	client->setClientWritingFlag(true);
}
