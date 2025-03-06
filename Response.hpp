#ifndef	RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"


class Response
{
private:
	std::string	_response;
	std::string	*_buffer;
	std::string	_path;
	std::string	_type;
public:
	Response();
	//Response(const Response &copy);
	//Response& operator=(const Response &copy);
	~Response();
	std::string	getResponse();
	std::string	*getBuffer();
	std::string	getPath();
	std::string	getType();
	void	setResponse(std::string response);
	void	setBuffer(std::string *buffer);
	void	setPath(std::string path);
	void	setType(std::string type);
	void	addToResponse(std::string info);
	std::string	*readFromBuffer();
};

#endif
