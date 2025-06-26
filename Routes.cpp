#include "includes/Routes.hpp"

Routes::Routes() : _maxConnections(-1), _maxBodySize(-1), _defaultRoute(false), _isCgi(false), _directoryListing(false), _isPermanentRedirect(false), _isTemporaryRedirect(false)
{
	for (int i = 0; i < 3; i++)
		this->_methods[i] = false;
	this->_savedPath = "";
}

Routes::Routes(int maxConnections, int maxBodySize, bool flag, std::string root, std::string uri) : _root(root), _uri(uri), _maxConnections(maxConnections), _maxBodySize(maxBodySize), _defaultRoute(flag), _isCgi(false), _directoryListing(false), _isPermanentRedirect(false), _isTemporaryRedirect(false)
{
	for (int i = 0; i < 3; i++)
		this->_methods[i] = false;
	this->_savedPath = "";
}

Routes::Routes(const Routes &copy)
{
	*this = copy;
}

Routes	&Routes::operator=(const Routes &copy)
{
	if (this != &copy)
	{
		this->_root = copy._root;
		this->_uri = copy._uri;
		this->_maxConnections = copy._maxConnections;
		this->_maxBodySize = copy._maxBodySize;
		for (size_t i = 0; i < 3; i++)
			this->_methods[i] = copy._methods[i];
	}
	return (*this);
}

Routes::~Routes()
{
}

std::string	&Routes::getRoot()
{
	return (this->_root);
}

std::string	&Routes::getURI()
{
	return (this->_uri);
}

std::string	&Routes::getRedirectPath()
{
	return (this->_redirectPath);
}

std::string	&Routes::getSavedPath()
{
	return (this->_savedPath);
}

int	Routes::getMaxConnections()
{
	return (this->_maxConnections);
}

int	Routes::getMaxBodySize()
{
	return (this->_maxBodySize);
}

bool	Routes::canDoMethod(int method)
{
	switch (method)
	{
		case 0:
			return (this->_methods[GET]);
		case 1:
			return (this->_methods[POST]);
		case 2:
			return (this->_methods[DELETE]);
		default:
			return (false);
	}
}

bool	Routes::isDefault()
{
	return (this->_defaultRoute);
}

bool	Routes::isCgi()
{
	return (this->_isCgi);
}

bool	Routes::isListing()
{
	return (this->_directoryListing);
}

bool	Routes::isPermanentRedirect()
{
	return (this->_isPermanentRedirect);
}

bool	Routes::isTemporaryRedirect()
{
	return (this->_isTemporaryRedirect);
}

void	Routes::setRoot(std::string &root)
{
	this->_root = root;
}

void	Routes::setURI(std::string &uri)
{
	this->_uri = uri;
}

void	Routes::setRedirectPath(std::string &path)
{
	this->_redirectPath = path;
}

void	Routes::setSavedPath(std::string &path)
{
	this->_savedPath = path;
}

void	Routes::setMaxConnections(int value)
{
	this->_maxConnections = value;
}

void	Routes::setMaxBodySize(int value)
{
	this->_maxBodySize = value;
}

void	Routes::setMethod(int method, bool flag)
{
	switch (method)
	{
		case 0:
			this->_methods[GET] = flag;
			break ;
		case 1:
			this->_methods[POST] = flag;
			break ;
		case 2:
			this->_methods[DELETE] = flag;
			break ;
	}
}

void	Routes::setAsCgi()
{
	this->_isCgi = true;
}

void	Routes::setAsListing()
{
	this->_directoryListing = true;
}

void	Routes::setAsPermanentRedirect()
{
	this->_isPermanentRedirect = true;
	this->_isTemporaryRedirect = false;
}

void	Routes::setAsTemporaryRedirect()
{
	this->_isTemporaryRedirect = true;
	this->_isPermanentRedirect = false;
}

std::string	generateListingHTML(std::string &dirPath)
{
	std::stringstream	html;

	html<<"<!DOCTYPE html>\n<html>\n<head>\n";
    html<<"<meta charset=\"UTF-8\">\n";
    html<<"<title>Index of " + dirPath + "</title>\n";
    html<<"</head>\n<body>\n";
	html<<"<h1>Index of " + dirPath + "</h1>\n";
    html<<"<ul>\n";

	DIR	*dir = opendir(dirPath.c_str());
	if (!dir)
		html<<"<p>Could not open directory.</p>\n";
	else
	{
		struct dirent* entry;

		while ((entry = readdir(dir)) != NULL)
		{
			std::string name = entry->d_name;
			//if (name == "." || name == "..")
            //    continue;
			std::string fullPath = dirPath + "/" + name;
            struct stat st;
            bool isDir = (stat(fullPath.c_str(), &st) == 0) && S_ISDIR(st.st_mode);
			html<<"<li><a href=\"" + name + "\">" + name;
            if (isDir)
				html<<"/";
            html<<"</a></li>\n";
		}
		closedir(dir);
	}
	html<<"</ul>\n</body>\n</html>\n";
	return (html.str());
}

LoadListingException::LoadListingException(int client_socket, Response *response, Client *client) :
runtime_error("") 
{
	std::string path;
	if (client->getRouteTriggered()->getSavedPath().empty())
	{
		path = client->getNewPath().substr(0, client->getNewPath().length() - 1);
		client->getRouteTriggered()->setSavedPath(path);
	}
	else
	{
		if (client->getRouteTriggered()->getURI().find(1, '/') != std::string::npos)
			path = client->getRouteTriggered()->getSavedPath() + client->getClientRequest()->get_path().substr(client->getRouteTriggered()->getURI().length());
		else
			path = client->getRouteTriggered()->getSavedPath() + client->getClientRequest()->get_path().substr(client->getRouteTriggered()->getURI().length() - 1);
		client->getRouteTriggered()->setSavedPath(path);
	}
	std::cout<<"SEARCHING FOR PATH: " + path<<std::endl;
	std::string	body = generateListingHTML(path);
	response->setStatusCode(200);
	response->clearResponse();
	response->addToResponse("HTTP/1.1 " + transformToString(response->getStatusCode()) + " OK\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n");
	else
		response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Type: text/html\r\n");
	ssize_t	bodyLenght = body.size();
	response->addToResponse("Content-Length: " + transformToString(bodyLenght) + "\r\n\r\n");
	sendMsgToSocket(client_socket, response->getResponse().size(), client, response);
	response->clearResponse();
	ssize_t	bytesSent = send(client_socket, body.c_str(), body.size(), MSG_NOSIGNAL);
	if (bytesSent == -1)
		throw SendException(client, response);
	response->addToBytesSent(bytesSent);
	client->setClientWritingFlag(true);
	client->setClientPending(false);
	//printLog("INFO", NULL, client, NULL, 301);
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}
