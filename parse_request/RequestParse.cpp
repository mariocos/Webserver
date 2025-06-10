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
	//std::cout << "Default RequestParse constructor called\n";
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
	if (path_to_request.find("?") != std::string::npos)
	{
		queryString = get_keyword(path_to_request, "?");
		path_to_request = path_to_request.substr(0, path_to_request.find("?"));
	}
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
	content_length = get_keyword(req, "Content-Length: ");
	content_type = get_keyword(req, "Content-Type: ");
	User = get_keyword(req, "User: ");
	Accepts = get_keyword(req, "Accept: ");
	connection = get_keyword(req, "Connection: ");
	Host = get_keyword(req, "Host: ");
	if (Host.find(":") != std::string::npos)
		Host.erase(Host.find(":"), 5);
	if (req.find("\r\n\r\n"))
	{
		req = req.erase(0, req.find("\r\n\r\n") + 4);
		content = req;
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
	i -= req.find(keyword) + keyword.length();
	return (req.substr(req.find(keyword) + keyword.length(), i));
}

std::string	RequestParse::get_buffer()
{
	return (*_buffer);
}

std::string	RequestParse::get_query_str()
{
	return (queryString);
}

RequestParse::~RequestParse()
{
}

void	RequestParse::set_path(std::string path)
{
	path_to_request = path;
}

void	RequestParse::setBuffer(std::string *buffer)
{
	_buffer = buffer;
}

void	RequestParse::setNewHost(std::string str)
{
	this->Host = str;
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

void	RequestParse::readBinary(int client_socket, Client *client)
{
	ssize_t	bytes_read;
	std::vector<uint8_t>	binaryBuffer(1048576);

	bytes_read = read(client_socket, reinterpret_cast<char*>(binaryBuffer.data()), 1048576);
	if (bytes_read == -1)
	{
		perror("read: ");
		close(client_socket);
	}
	if (this->_binaryBuffer.empty())
		this->_binaryBuffer.assign(binaryBuffer.begin(), binaryBuffer.begin() + bytes_read);
	else
	{
		std::vector<uint8_t>::iterator	it = binaryBuffer.begin();
		while (it != binaryBuffer.end())
		{
			this->_binaryBuffer.push_back(*it);
			it++;
		}
	}
	if (bytes_read >= 1048574)
		client->setClientReadingFlag(false);
	else
		client->setClientReadingFlag(true);
}

void	RequestParse::clearBuffer()
{
	this->_binaryBuffer.clear();
}

std::vector<uint8_t>	&RequestParse::getBufferInfo()
{
	return (this->_binaryBuffer);
}

void	RequestParse::execute_response(int client_socket, Client *client)
{
	if (method.compare("GET") == 0)
		GET_response(client_socket, client);
	else if (method.compare("POST") == 0)
	{	
		Post_master::post(client);
		POST_response(client_socket, client);
		client->setClientWritingFlag(true);
		client->setClientPending(false);
	}
	else if (method.compare("DELETE") == 0)
	{
		DELETE_response(client_socket, client);
		client->setClientWritingFlag(true);
		client->setClientPending(false);
	}
	else
		throw Error405Exception(client_socket, client->getClientResponse(), client);
}

void	RequestParse::GET_response(int client_socket, Client *client)
{
	//creating the header for the response and oppening the file requested
	if (!client->getClientFile()->isReading() && !client->getClientFile()->isWriting())
	{
		findType(this, client->getClientResponse());
		client->getClientFile()->openFile(client->getClientResponse()->getPath().c_str(), client_socket);
		createHeader(this, client->getClientResponse(), client);
	}

	//reading/write operations
	if (client->getClientFile()->isReading())
	{
		std::cout<<YELLOW<<"Is Reading From the file"<<RESET<<std::endl;
		if (client->getClientFile()->getFileStats()->st_size > 1048576)
			client->getClientFile()->readFromFd(1048576);
		else
			client->getClientFile()->readFromFd(client->getClientFile()->getFileStats()->st_size);
	}
	else if (client->getClientFile()->isWriting())
	{
		std::cout<<YELLOW<<"Is Writing to the socket"<<RESET<<std::endl;
		loadPage(client_socket, client->getClientResponse(), client);
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

std::string RequestParse::get_content()
{
	return (content);
}