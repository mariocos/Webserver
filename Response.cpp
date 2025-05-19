#include "includes/Response.hpp"


Response::Response() : _response(), _path(""), _type("text/plain"), _totalResponseLenght(0), _bytesSent(0)
{
	this->_buffer = NULL;
	for (size_t i = 0; i < this->_response.size(); i++)
	{
		this->_response[i] = '\0';
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

std::vector<char>	&Response::getResponse()
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
	std::ostringstream	number;
	std::string	lenght;

	number << this->_totalResponseLenght;
	lenght = number.str();
	return (lenght);
}

unsigned int Response::getResponseLenght()
{
	return (this->_totalResponseLenght);
}

ssize_t	Response::getBytesSent()
{
	return (this->_bytesSent);
}

void	Response::setResponse(const std::vector<char>& response)
{
	this->_response = response;
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

void	Response::addToResponse(std::string info)
{
	this->_response.insert(this->_response.end(), info.begin(), info.end());
}

void	Response::addToResponseLenght(unsigned int bytes)
{
	this->_totalResponseLenght += bytes;
}

void	Response::addToBytesSent(ssize_t bytes)
{
	this->_bytesSent += bytes;
}

void	Response::clearResponse()
{
	this->_response.clear();
}

std::string	Response::readFromBuffer()
{
	return (this->_buffer ? *_buffer : "");
}
