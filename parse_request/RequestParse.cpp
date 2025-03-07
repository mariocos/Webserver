#include "RequestParse.hpp"

RequestParse::RequestParse()
{
	std::cout << "Default RequestParse constructor called\n";
}

RequestParse::RequestParse(const char *request)
{
	this->buildRequest(request);
}

void	RequestParse::buildRequest(const char *request)
{
	error_code = 0;
	std::cout << "calling request parse cstring constructor\n";
	if (!request)
	{
		std::cout << "im getting a null cstring\n";
		return ;//not sure what to do but im pretty sure this gets caught earlier
	}
	std::string	req(request);
	/* first line */
	std::string line1 = req.substr(0, req.find('\n'));
	int	len = line1.find(' ');
	this->method = line1.substr(0, len);
	line1.erase(0, len + 1);
	len = line1.find(' ');
	this->path_to_request = line1.substr(0, len);
	if (path_to_request.find("../"))
	{
		std::cout << "naughty request\n";
		error_code = 3;
	}
	line1.erase(0, len + 1);
	this->HTTP_version = line1.substr(0, line1.find('\r'));
	req.erase(0, req.find('\n') + 1);
	/* key word get */
	content_length = get_keyword(req, "content-length: ");
	content_type = get_keyword(req, "content-type: ");
	User = get_keyword(req, "User: ");
	Accepts = get_keyword(req, "Accept: ");
	connection = get_keyword(req, "Connection: ");
}


std::string	get_keyword(std::string req, std::string keyword)
{
	int	i = 0;

	if (!req.find(keyword))
		return ("");
	i = req.find(keyword) + keyword.length();
	while (req[i] != '\r')
		i++;
	return (req.substr(req.find(keyword) + keyword.length(), i));
}

RequestParse::~RequestParse()
{
	std::cout<<RED<<"Destructor"<<RESET<<std::endl;
}

void	RequestParse::set_path(std::string path)
{
	path_to_request = path;
}

void	RequestParse::setBuffer(std::string *buffer)
{
	_buffer = buffer;
}

void	RequestParse::writeToBuffer(std::string info)
{
	adjustBuffer();
	_buffer->append(info);
}

void	RequestParse::adjustBuffer()
{
	if (_buffer)
		_buffer->insert(0, *_buffer);
}

bool	RequestParse::execute_response(int client_socket, Client *client)
{
	if (method.compare("GET") == 0)
		return (GET_response(client_socket, client));
	else if (method.compare("POST") == 0)
		return (POST_response(client_socket, client));
	else if (method.compare("DELETE") == 0)
		return (DELETE_response(client_socket, client));
	else
		std::cout<<"ERROR"<<std::endl;
	return (false);
}

bool	RequestParse::GET_response(int client_socket, Client *client)
{
	if (client->getClientPending() == false)
	{
		findType(this, client->getClientResponse());
		createHeader(this, client->getClientResponse(), client);
	}
	else
		findType(this, client->getClientResponse());
	if (client->getClientResponse()->getType() == "image/png")
		return (loadImgResponse(client_socket, client->getClientResponse(), client));
	else
	{
		int	fd = -1;
		if (client->getClientOpenFd() == -1)
		{
			fd = open(client->getClientResponse()->getPath().c_str(), O_RDONLY);
			std::cout<<"client_socket: "<<client->getClientSocket()<<std::endl;
			if (fd != -1 && setNonBlocking(fd) == -1)
			{
    		    std::cerr << "Failed to set non-blocking mode." << std::endl;
    		    close(fd);
    		    return (false);
    		}
		}
		else
			fd = client->getClientOpenFd();
		std::cout<<"fd: "<<fd<<std::endl;
		if (fd == -1)
			return (loadErrorPage(client_socket, client->getClientResponse(), client));
		else
			return (loadPage(client_socket, fd, client->getClientResponse(), client));
	}
	return (false);
}

bool	RequestParse::POST_response(int client_socket, Client *client)
{
	(void)client_socket;
	return (client->getClientPending());
}

bool	RequestParse::DELETE_response(int client_socket, Client *client)
{
	(void)client_socket;
	return (client->getClientPending());
}
