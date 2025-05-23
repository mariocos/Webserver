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

void	Cgi::setCgiResponse(unsigned int buffer_size)
{
	std::vector<uint8_t>	binaryBuffer(buffer_size);
	ssize_t	bytesRead = 1;

	while (bytesRead > 0)
	{
		bytesRead = read(this->_cgiStdOut[0], reinterpret_cast<char*>(binaryBuffer.data()), buffer_size);
		this->_cgiResponse.insert(this->_cgiResponse.end(), binaryBuffer.begin(), binaryBuffer.begin() + bytesRead);
	}
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
	std::string	path = "website" + client->getClientRequest()->get_path();
	std::vector<char*>	env;
	for (size_t i = 0; i < this->_envp.size(); i++)
	{
		env.push_back(const_cast<char*>(this->_envp[i].c_str()));
	}
	env.push_back(NULL);
	dup2(this->_cgiStdIn[0], STDIN_FILENO);
	dup2(this->_cgiStdOut[1], STDOUT_FILENO);
	close(this->_cgiStdIn[1]);
	close(this->_cgiStdOut[0]);
	char	*av[] = {(char*)"/usr/bin/python3", const_cast<char*>(path.c_str()), NULL};
	execve("/usr/bin/python3", av, env.data());
	perror("execve failed");
	exit(1);
}

void	Cgi::readCgiResponse(Server &server, Client *client)
{
	this->setCgiResponse(1048576);
	epoll_ctl(server.getEpollFd(), EPOLL_CTL_DEL, this->_cgiStdOut[0], NULL);
	close(this->_cgiStdOut[0]);
	this->changeCgiState();
	client->setClientReadingFlag(false);
	client->setClientWritingFlag(true);
}

void	Cgi::writeCgiResponse(Client *client)
{
	client->getClientResponse()->clearResponse();
	client->getClientResponse()->addToResponse(client->getClientRequest()->get_httpversion() + " 200 OK\r\n");
	client->getClientResponse()->addToResponse("Content-Type: " + client->getClientResponse()->getType() + "\r\n");
	client->getClientResponse()->addToResponseLenght(this->_cgiResponse.size());
	client->getClientResponse()->addToResponse("Connection: close\r\n");
	client->getClientResponse()->addToResponse("Content-Length: " + client->getClientResponse()->getResponseLenghtAsString() + "\r\n\r\n");
	sendMsgToSocket(client->getSocketFd(), client->getClientResponse()->getResponse().size(), client, client->getClientResponse());
	client->getClientResponse()->clearResponse();
	std::vector<uint8_t> data = this->_cgiResponse;
	if (send(client->getSocketFd(), reinterpret_cast<const char*>(data.data()), this->_cgiResponse.size(), MSG_NOSIGNAL) == -1)
		throw SendException(client, client->getClientResponse());
	client->setClientReadingFlag(true);
	client->setClientWritingFlag(true);
}
