#include "File.hpp"

File::File() : _fd(-1), _client(NULL), _checkingSize(true), _isReading(false), _isWriting(false)
{
	this->_buffer = new std::string;
}

File::File(int fd, Client *client) : _fd(fd), _client(client), _checkingSize(true), _isReading(false), _isWriting(false)
{
	this->_buffer = new std::string;
}

File::~File()
{
	delete this->_buffer;
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
	return (this->_buffer ? *_buffer : "");
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
	this->_buffer->clear();
}

void	File::writeToBuffer(char *info)
{
	adjustBuffer();
	this->_buffer->append(info);
}

void	File::adjustBuffer()
{
	if (this->_buffer)
		this->_buffer->insert(0, *this->_buffer);
}

void	File::readFromFd()
{
	if (this->_buffer->empty())
	{
		char	buffer[1024];
		this->_bytesRead = 1;
		ft_bzero(buffer, sizeof(buffer));
		_bytesRead = read(this->_fd, buffer, sizeof(buffer));
		//std::cout<<"Read From fd Buffer: \n"<<buffer<<std::endl;
		std::cout<<"Bytes read: \n"<<_bytesRead<<std::endl;
		this->writeToBuffer(buffer);
		if (_bytesRead >= 1022)
			this->getClient()->setClientOpenFd(this->_fd);
		this->_isReading = false;
		this->_isWriting = true;
	}
}
