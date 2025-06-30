#ifndef	RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

class Client;

class Response
{
private:
	std::vector<uint8_t>	_response;
	std::vector<uint8_t>	_binaryBuffer;
	std::string	*_buffer;
	std::string	_path;
	std::string	_type;
	unsigned int	_totalResponseLenght;
	ssize_t		_bytesToSend;
	ssize_t		_bytesSent;
	int			_statusCode;
public:
	Response();
	//Response(const Response &copy);
	//Response& operator=(const Response &copy);
	~Response();
	std::vector<uint8_t> &getResponse();
	std::string	getPath();
	std::string	getType();
	std::string	*getBuffer();
	std::string	getResponseLenghtAsString();
	unsigned int getResponseLenght();
	ssize_t		getBytesToSend();
	ssize_t		getBytesSent();
	int			getStatusCode();
	void	setResponse(std::vector<uint8_t> &response);
	void	setBinaryBuffer(std::vector<uint8_t> &buffer);
	void	setBuffer(std::string *buffer);
	void	setPath(std::string path);
	void	setType(std::string type);
	void	setStatusCode(int status);
	void	addToResponse(std::string info);
	void	addToResponseLenght(unsigned int bytes);
	void	addToBytesToSend(ssize_t bytes);
	void	addToBytesSent(ssize_t bytes);
	void	resetResponseLenght();
	void	resetBytesToSend();
	void	resetBytesSent();
	void	clearResponse();
	std::string	readFromBuffer();
	std::vector<uint8_t> &readFromBinaryBuffer();
};

#endif
