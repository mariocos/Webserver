#include "includes/File.hpp"

File::File() : _fd(-1), _bytesRead(0), _client(NULL), _buffer(""), _checkingSize(true), _isReading(false), _isWriting(false)
{
}

File::File(int fd, Client *client) : _fd(fd), _bytesRead(0), _client(client), _buffer(""), _checkingSize(true), _isReading(false), _isWriting(false)
{
}

File::~File()
{
	if (this->_fd != -1)
		close(this->_fd);
}

int	File::getFd()
{
	return (this->_fd);
}

Client	*File::getClient()
{
	return (this->_client);
}

std::string File::readFromBuffer()
{
	return (this->_buffer);
}

bool	File::getCheckingSizeFlag()
{
	return (this->_checkingSize);
}

bool	File::isReading()
{
	return (this->_isReading);
}

bool	File::isWriting()
{
	return (this->_isWriting);
}

unsigned int	File::getBytesRead()
{
	return (this->_bytesRead);
}

void	File::setFd(int fd)
{
	this->_fd = fd;
}

void	File::setClient(Client *client)
{
	this->_client = client;
}

void	File::setCheckingSizeFlag(bool flag)
{
	this->_checkingSize = flag;
}

void	File::setReading(bool flag)
{
	this->_isReading = flag;
}

void	File::setWriting(bool flag)
{
	this->_isWriting = flag;
}

void	File::clearBuffer()
{
	this->_buffer.clear();
}

void	File::writeToBuffer(char *info)
{
	adjustBuffer();
	this->_buffer.append(info);
}

void	File::adjustBuffer()
{
	if (!this->_buffer.empty())
		this->_buffer.insert(0, this->_buffer);
}

void	File::readFromFd(unsigned int buffer_size)
{
	if (this->_buffer.empty())
	{
		std::string	binaryBuffer(buffer_size, '\0');

		this->_bytesRead = read(this->_fd, &binaryBuffer[0], buffer_size);
		//std::cout<<"Read From fd Buffer: \n"<<buffer<<std::endl;
		//std::cout<<"Bytes read: \n"<<_bytesRead<<std::endl;
		this->_buffer = binaryBuffer;
		if (this->_bytesRead >= buffer_size)
			this->getClient()->setClientOpenFd(this->_fd);
		this->_isReading = false;
		this->_isWriting = true;
	}
}
