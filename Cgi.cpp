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

std::vector<char*>	&Cgi::getEnv()
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

void	Cgi::setEnv(std::vector<char*> env)
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
