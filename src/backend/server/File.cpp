#include "../../../includes/File.hpp"

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

void	File::closeFile()
{
	if (this->_file.is_open())
		this->_file.close();
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
		if (errno == EACCES || errno == EPERM)
			throw Error403Exception(client_socket, this->_client->getClientResponse(), this->_client);
		std::string	file_path;
		if (!this->_client->getRouteTriggered()->getDefaultFile().empty())
			file_path = this->_client->getRouteTriggered()->getDefaultPathForDirectoryRequest();
		else
			file_path = "";
		this->_file.open(file_path.c_str(), std::ios::in);
		if (!this->_file.is_open())
		{
			if (errno == EACCES || errno == EPERM)
				throw Error403Exception(client_socket, this->_client->getClientResponse(), this->_client);
			else
				throw Error404Exception(client_socket, this->_client->getClientResponse(), this->_client);
		}
		if (stat(file_path.c_str(), &this->_fileStats))
		{
			if (errno == EACCES || errno == EPERM)
				throw Error403Exception(client_socket, this->_client->getClientResponse(), this->_client);
			else
				throw Error404Exception(client_socket, this->_client->getClientResponse(), this->_client);
		}
		this->_client->getClientResponse()->setType(getFileType(file_path));
		this->_client->getClientResponse()->setPath(file_path);
		this->_client->getClientRequest()->set_path(file_path);
		return (false);
	}
	return (true);
}

std::string	getFileType(std::string path)
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
	fileTypes[".pdf"] = "application/pdf";

	std::string	extension = findFileExtension(path);
	std::string	type = "application/octet-stream";

	if (fileTypes.count(extension))
		type = fileTypes.at(extension);
	return (type);
}
