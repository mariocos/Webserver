#include "RequestParse.hpp"

char	*ft_strstr(const char *big, const char *little)
{
	size_t	j;
	size_t	i;

	j = 0;
	i = 0;
	if (!*little)
		return ((char *)big);
	while (*big)
	{
		j = 0;
		while (big[j] && little[j] && big[j] == little[j])
		{
			j++;
		}
		if (little[j] == '\0')
			return ((char *)big);
		big++;
		i++;
	}
	return (0);
}

RequestParse::RequestParse() : _buffer(NULL)
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
	std::cout << "path to request is: [" << path_to_request << "]\n";
	if (ft_strstr(path_to_request.c_str(), ".."))
	{
		std::cout<< "what is naughty about this [" << path_to_request.find("..") << "]\n";
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
	if (req.find("\r\n\r\n"))
	{
		req.erase(0, req.find("\r\n\r\n") + 4);
		content = req.substr(0, req.find("\0") + 1);
	}
}


std::string	get_keyword(std::string req, std::string keyword)
{
	int	i = 0;

	if (!ft_strstr(req.c_str(), keyword.c_str()))
		return ("");
	i = req.find(keyword) + keyword.length();
	while (req[i] && req[i] != '\r')
		i++;
	return (req.substr(req.find(keyword) + keyword.length(), i));
}

std::string	RequestParse::get_buffer()
{
	return (*_buffer);
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

void	RequestParse::writeToBuffer(char *info)
{
	adjustBuffer();
	_buffer->append(info);
}

void	RequestParse::adjustBuffer()
{
	if (_buffer)
		_buffer->insert(0, *_buffer);
}

void	RequestParse::readToBuffer(int client_socket, Client *client)
{
	ssize_t	bytes_read;
	char	buffer[1024];

	ft_bzero(buffer, 1024);
	bytes_read = read(client_socket, buffer, sizeof(buffer));
	if (bytes_read == -1)
	{
		perror("read: ");
		close(client_socket);
	}
	std::cout<<"Buffer:\n"<<buffer<<std::endl;
	writeToBuffer(buffer);
	if (bytes_read >= 1022)
		client->setClientReadingFlag(false);
	else
		client->setClientReadingFlag(true);
}

void	RequestParse::execute_response(int client_socket, Client *client)
{
	if (method.compare("GET") == 0)
		GET_response(client_socket, client);
	else if (method.compare("POST") == 0)
		POST_response(client_socket, client);
	else if (method.compare("DELETE") == 0)
		DELETE_response(client_socket, client);
	else
		throw Error400Exception(client_socket, client);
}

void	RequestParse::GET_response(int client_socket, Client *client)
{
	if (client->getClientWritingFlag() == false && client->getClientPending() == false)
	{
		findType(this, client->getClientResponse());
		client->getClientResponse()->checkHowManyBytesToSend(client_socket, client);
		createHeader(this, client->getClientResponse(), client);
	}
	else if (client->getClientPending() == true)
		findType(this, client->getClientResponse());
	if (client->getClientResponse()->getType() == "image/png")
		loadImgResponse(client_socket, client->getClientResponse(), client);
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
    		    return ;
    		}
		}
		else
			fd = client->getClientOpenFd();
		if (fd == -1)
			throw Error404Exception(client_socket, client->getClientResponse(), client);
		else
			loadPage(client_socket, fd, client->getClientResponse(), client);
	}
}

void	RequestParse::POST_response(int client_socket, Client *client)
{
	(void)client_socket;
	(void)client;
}

void	RequestParse::DELETE_response(int client_socket, Client *client)
{
	(void)client_socket;
	(void)client;
}
