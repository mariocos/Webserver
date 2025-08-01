#include "includes/Response.hpp"


Response::Response() : _response(), _path(""), _type("text/plain"), _totalResponseLenght(0), _bytesToSend(0), _bytesSent(0)
{
	this->_buffer = NULL;
	for (size_t i = 0; i < this->_response.size(); i++)
	{
		this->_response[i] = 0;
	}
}

//Response::Response(const Response &copy)
//{
//}

Response::~Response()
{
}

//Response&	Response::operator=(const Response &copy)
//{
//	return (*this);
//}

std::vector<uint8_t>	&Response::getResponse()
{
	return (this->_response);
}

std::string	*Response::getBuffer()
{
	return (this->_buffer);
}

std::string	Response::getPath()
{
	return (this->_path);
}

std::string	Response::getType()
{
	return (this->_type);
}

std::string	Response::getResponseLenghtAsString()
{
	return (transformToString(this->_totalResponseLenght));
}

unsigned int Response::getResponseLenght()
{
	return (this->_totalResponseLenght);
}

ssize_t	Response::getBytesToSend()
{
	return (this->_bytesToSend);
}

ssize_t	Response::getBytesSent()
{
	return (this->_bytesSent);
}

int	Response::getStatusCode()
{
	return (this->_statusCode);
}

void	Response::setResponse(std::vector<uint8_t> &response)
{
	this->_response = response;
}

void	Response::setBinaryBuffer(std::vector<uint8_t> &buffer)
{
	if (this->_binaryBuffer.empty())
		this->_binaryBuffer = buffer;
	else
	{
		std::vector<uint8_t>::iterator	it = buffer.begin();
		while (it != buffer.end())
		{
			this->_binaryBuffer.push_back(*it);
			it++;
		}
	}
}

void	Response::setBuffer(std::string *buffer)
{
	this->_buffer = buffer;
}

void	Response::setPath(std::string path)
{
	this->_path = path;
}

void	Response::setType(std::string type)
{
	this->_type = type;
}

void	Response::setStatusCode(int status)
{
	this->_statusCode = status;
}

void	Response::addToResponse(std::string info)
{
	this->_response.insert(this->_response.end(), info.begin(), info.end());
}

void	Response::addToResponseLenght(unsigned int bytes)
{
	this->_totalResponseLenght += bytes;
}

void	Response::addToBytesToSend(ssize_t bytes)
{
	this->_bytesToSend += bytes;
}

void	Response::addToBytesSent(ssize_t bytes)
{
	this->_bytesSent += bytes;
}

void	Response::resetResponseLenght()
{
	this->_totalResponseLenght = 0;
}

void	Response::resetBytesToSend()
{
	this->_bytesToSend = 0;
}

void	Response::resetBytesSent()
{
	this->_bytesSent = 0;
}

void	Response::clearResponse()
{
	this->_response.clear();
}

std::string	Response::readFromBuffer()
{
	return (this->_buffer ? *_buffer : "");
}

std::vector<uint8_t> &Response::readFromBinaryBuffer()
{
	return (this->_binaryBuffer);
}
