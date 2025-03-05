#include "Response.hpp"


Response::Response() : _type("text/plain")
{
	std::cout<<GREEN<<"Response default constructor called"<<RESET<<std::endl;
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

std::string	Response::getResponse()
{
	return (this->_response);
}

std::string	Response::getBuffer()
{
	return (this->_r_buffer);
}

std::string	Response::getPath()
{
	return (this->_path);
}

std::string	Response::getType()
{
	return (this->_type);
}

void	Response::setResponse(std::string response)
{
	this->_response = response;
}

void	Response::setBuffer(std::string buffer)
{
	this->_r_buffer = buffer;
}

void	Response::setPath(std::string path)
{
	this->_path = path;
}

void	Response::setType(std::string type)
{
	this->_type = type;
}
