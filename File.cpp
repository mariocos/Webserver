#include "includes/File.hpp"

File::File() : _bytesRead(0), _client(NULL), _buffer(""), _checkingSize(true), _isReading(false), _isWriting(false)
{
}

File::File(Client *client) : _bytesRead(0), _client(client), _buffer(""), _checkingSize(true), _isReading(false), _isWriting(false)
{
}

File::~File()
{
	if (this->_file.is_open())
		this->_file.close();
}

std::fstream	*File::getFile()
{
	return (&this->_file);
}

struct stat	*File::getFileStats()
{
	return (&this->_fileStats);
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

		this->_file.read(&binaryBuffer[0], buffer_size);
		this->_bytesRead = this->_file.gcount();
		//std::cout<<"Read From fd Buffer: \n"<<buffer<<std::endl;
		//std::cout<<"Bytes read: \n"<<_bytesRead<<std::endl;
		this->_buffer = binaryBuffer;
		this->_isReading = false;
		this->_isWriting = true;
	}
}
