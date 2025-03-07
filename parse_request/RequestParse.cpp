#include "RequestParse.hpp"

RequestParse::RequestParse()
{
	std::cout << "Default RequestParse constructor called\n";
}

RequestParse::RequestParse(const char *request)
{
	std::cout << "calling request parse cstring constructor\n";
	if (!request)
	{
		std::cout << "im getting a null cstring\n";
		return ;//not sure what to do but im pretty sure this gets caught earlier
	}
	std::string	req(request);

	/* first line */
	req.find('\n');
	std::string line1 = req.substr(0, req.find('\n'));
	int	len = line1.find(' ');
	this->method = line1.substr(0, len);
	line1.erase(0, len + 1);
	len = line1.find(' ');
	this->path_to_request = line1.substr(0, len);
	line1.erase(0, len + 1);
	this->HTTP_version = line1.substr(0, line1.find('\r'));
	req.erase(0, req.find('\n') + 1);
	/*  line 2   */
	line1 = req.substr(0, req.find('\n'));
	this->Host = line1.substr(line1.find(' ') + 1, line1.find('\r') - (line1.find(' ') + 1));
	req.erase(0, req.find('\n') + 1);//plus 1 for after the newline
	/* line 3 */
	line1 = req.substr(0, req.find('\n'));
	this->User = line1.substr(line1.find(' ') + 1, line1.find('\r') - (line1.find(' ') + 1));
	req.erase(0, req.find('\n') + 1);
	/* line 4*/
	line1 = req.substr(0, req.find('\n'));
	this->Accepts = line1.substr(line1.find(' ') + 1, line1.find('\r') - (line1.find(' ') + 1));
}


RequestParse::~RequestParse()
{
	std::cout<<RED<<"Destructor"<<RESET<<std::endl;
}

std::string	RequestParse::get_method(void)
{
	return (method);
}

std::string	RequestParse::get_host(void)
{
	return (Host);
}
std::string	RequestParse::get_httpversion(void)
{
	return (HTTP_version);
}
std::string	RequestParse::get_user(void)
{
	return (User);
}
std::string	RequestParse::get_accetps(void)
{
	return (Accepts);
}

std::string	RequestParse::get_path(void)
{
	return (path_to_request);
}

void	RequestParse::buildRequest(const char *request)
{
	if (!request)
	{
		std::cout << "im getting a null cstring\n";
		return ;//not sure what to do but im pretty sure this gets caught earlier
	}
	std::string	req(request);

	/* first line */
	req.find('\n');
	std::string line1 = req.substr(0, req.find('\n'));
	int	len = line1.find(' ');
	this->method = line1.substr(0, len);
	line1.erase(0, len + 1);
	len = line1.find(' ');
	this->path_to_request = line1.substr(0, len);
	line1.erase(0, len + 1);
	this->HTTP_version = line1.substr(0, line1.find('\r'));
	req.erase(0, req.find('\n') + 1);
	/*  line 2   */
	line1 = req.substr(0, req.find('\n'));
	this->Host = line1.substr(line1.find(' ') + 1, line1.find('\r') - (line1.find(' ') + 1));
	req.erase(0, req.find('\n') + 1);//plus 1 for after the newline
	/* line 3 */
	line1 = req.substr(0, req.find('\n'));
	this->User = line1.substr(line1.find(' ') + 1, line1.find('\r') - (line1.find(' ') + 1));
	req.erase(0, req.find('\n') + 1);
	/* line 4*/
	line1 = req.substr(0, req.find('\n'));
	this->Accepts = line1.substr(line1.find(' ') + 1, line1.find('\r') - (line1.find(' ') + 1));
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
