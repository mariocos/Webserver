#include "includes/Response.hpp"


Response::Response() : _response(""), _path(""), _type("text/plain"), _totalResponseLenght(0), _bytesSent(0)
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

void	Response::setResponse(std::string response)
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
	this->_response.append(info);
}

void	Response::addToResponseLenght(unsigned int bytes)
{
	this->_totalResponseLenght += bytes;
}

void	Response::addToBytesSent(ssize_t bytes)
{
	this->_bytesSent += bytes;
}

std::string	Response::readFromBuffer()
{
	return (this->_buffer ? *_buffer : "");
}

void	Response::checkHowManyBytesToSend(int client_socket, Client *client)
{
	int	input;
	std::string	buffer(4096, '\0');
	ssize_t	bytes = 1;

	input = client->getClientFile()->getFd();
	if (input != -1)
	{
		std::cout<<YELLOW<<"Checking how many Bytes to send"<<RESET<<std::endl;
		while (bytes > 0)
		{
			bytes = read(input, &buffer[0], 4096);
			if (bytes <= 0)
				break ;
			addToResponseLenght(bytes);
		}
	}
	else
		throw Error404Exception(client_socket, this, client);
	close(input);
	client->getClientFile()->setCheckingSizeFlag(false);
	client->getClientFile()->setFd(open(client->getClientResponse()->getPath().c_str(), O_RDONLY | O_NONBLOCK));
}
