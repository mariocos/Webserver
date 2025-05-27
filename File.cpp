#include "includes/File.hpp"

File::File() : _bytesRead(0), _client(NULL), _isReading(false), _isWriting(false)
{
}

File::File(Client *client) : _bytesRead(0), _client(client), _isReading(false), _isWriting(false)
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

std::vector<uint8_t>	&File::readFromBuffer()
{
	return (this->_buffer);
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

void	File::writeToBuffer(uint8_t* info, size_t len)
{
    adjustBuffer();
    this->_buffer.insert(this->_buffer.end(), info, info + len);
}

void	File::adjustBuffer()
{
    if (!this->_buffer.empty())
	{
		std::vector<uint8_t> copy = this->_buffer;
		this->_buffer.insert(this->_buffer.begin(), copy.begin(), copy.end());
    }
}

void	File::readFromFd(unsigned int buffer_size)
{
	if (this->_buffer.empty())
	{
		std::vector<uint8_t>	binaryBuffer(buffer_size);

		this->_file.read(reinterpret_cast<char*>(binaryBuffer.data()), buffer_size);
		this->_bytesRead = this->_file.gcount();
		this->_buffer.assign(binaryBuffer.begin(), binaryBuffer.begin() + this->_bytesRead);
		this->_isReading = false;
		this->_isWriting = true;
	}
}

void	File::openFile(const char *path, int client_socket)
{
	if (!this->checkFileInfo(path, client_socket))
		return ;
	this->_file.open(path, std::ios::in | std::ios::binary);
	if (!this->_file.is_open())
	{
		if (errno == EACCES || errno == EPERM)
			throw Error403Exception(client_socket, this->_client->getClientResponse(), this->_client);
		else
			throw Error404Exception(client_socket, this->_client->getClientResponse(), this->_client);
	}
	if (!this->_file.good())
	{
		this->_file.close();
		throw Error404Exception(client_socket, this->_client->getClientResponse(), this->_client);
	}
}

bool	File::checkFileInfo(const char *path, int client_socket)
{
	if (stat(path, &this->_fileStats))
	{
		if (errno == EACCES || errno == EPERM)
			throw Error403Exception(client_socket, this->_client->getClientResponse(), this->_client);
		else
			throw Error404Exception(client_socket, this->_client->getClientResponse(), this->_client);
	}
	if (S_ISDIR(this->_fileStats.st_mode))
	{
		this->_file.open("website/index.html", std::ios::in);
		if (!this->_file.is_open())
		{
			if (errno == EACCES || errno == EPERM)
				throw Error403Exception(client_socket, this->_client->getClientResponse(), this->_client);
			else
				throw Error404Exception(client_socket, this->_client->getClientResponse(), this->_client);
		}
		if (stat("website/index.html", &this->_fileStats))
		{
			if (errno == EACCES || errno == EPERM)
				throw Error403Exception(client_socket, this->_client->getClientResponse(), this->_client);
			else
				throw Error404Exception(client_socket, this->_client->getClientResponse(), this->_client);
		}
		this->_client->getClientResponse()->setPath("website/index.html");
		this->_client->getClientRequest()->set_path("website/index.html");
		return (false);
	}
	return (true);
}
