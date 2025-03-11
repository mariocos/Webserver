#include "Response.hpp"


Response::Response() : _response(""), _path(""), _type("text/plain")
{
	std::cout<<GREEN<<"Response default constructor called"<<RESET<<std::endl;
	_buffer = NULL;
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
	return (_response);
}

std::string	*Response::getBuffer()
{
	return (_buffer);
}

std::string	Response::getPath()
{
	return (_path);
}

std::string	Response::getType()
{
	return (_type);
}

void	Response::setResponse(std::string response)
{
	_response = response;
}

void	Response::setBuffer(std::string *buffer)
{
	_buffer = buffer;
}

void	Response::setPath(std::string path)
{
	_path = path;
}

void	Response::setType(std::string type)
{
	_type = type;
}

void	Response::addToResponse(std::string info)
{
	_response.append(info);
}

std::string	Response::readFromBuffer()
{
	return (_buffer ? *_buffer : "");
}
