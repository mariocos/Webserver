#include "../../../includes/Routes.hpp"

Routes::Routes() : _maxBodySize(-1), _defaultRoute(false), _isCgi(false), _directoryListing(false), _isPermanentRedirect(false), _isTemporaryRedirect(false)
{
	for (int i = 0; i < 3; i++)
		this->_methods[i] = false;
	this->_savedPath = "";
	this->_cgiFileExtension = "";
	this->_defaultFileForDirectory = "";
	this->_uploadPath = "";
}

Routes::Routes(int maxBodySize, bool flag, std::string root, std::string uri) : _root(root), _uri(uri), _maxBodySize(maxBodySize), _defaultRoute(flag), _isCgi(false), _directoryListing(false), _isPermanentRedirect(false), _isTemporaryRedirect(false)
{
	if (root.rfind("/") == root.length() - 1)
		this->_root = root.substr(0, root.length() - 1);
	for (int i = 0; i < 3; i++)
		this->_methods[i] = false;
	this->_savedPath = "";
	this->_cgiFileExtension = "";
	this->_defaultFileForDirectory = "";
	this->_uploadPath = "";
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

std::string	&Routes::getCgiFileExtension()
{
	return (this->_cgiFileExtension);
}

std::string	&Routes::getDefaultFile()
{
	return (this->_defaultFileForDirectory);
}

std::string	Routes::getDefaultPathForDirectoryRequest()
{
	if (this->_root.rfind('/') != this->_root.length())
		return (this->_root + "/" + this->_defaultFileForDirectory);
	return (this->_root + this->_defaultFileForDirectory);
}

std::string	&Routes::getUploadPath()
{
	return (this->_uploadPath);
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

void	Routes::setCgiFileExtension(std::string &extension)
{
	this->_cgiFileExtension = extension;
}

void	Routes::setDefaultFileForDirectory(std::string &file_name)
{
	this->_defaultFileForDirectory = file_name;
}

void	Routes::setUploadPath(std::string &path)
{
	this->_uploadPath = path;
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

void		Routes::setAsDefaultRoute()
{
	this->_defaultRoute = true;
}

std::string	generateListingHTML(std::string &dirPath, Client *client)
{
	std::stringstream	html;

	html<<"<!DOCTYPE html>\n";
	html<<"<html>\n";
	html<<"<head>\n";
    html<<"<meta charset=\"UTF-8\">\n";
    html<<"<title>Index of " + dirPath + "</title>\n";
    html<<"</head>\n";
	html<<"<body>\n";
	html<<"<h1>Index of " + dirPath + "</h1>\n";

	if (dirPath != "/" && dirPath != "." && \
		dirPath != client->getRouteTriggered()->getRoot() && dirPath != client->getRouteTriggered()->getRoot() + "/")
	{
		std::string	parentDir = client->getClientRequest()->get_path().substr(0, client->getClientRequest()->get_path().rfind('/') + 1);
		if (parentDir == client->getRouteTriggered()->getURI())
			html<<"<p><a href=\"" + parentDir + "\"> Go to parent directory</a></p>\n";
		else
			html<<"<p><a href=\"" + parentDir.substr(0, parentDir.length() - 1) + "\"> Go to parent directory</a></p>\n";
	}
    html<<"<ul>\n";

	DIR	*dir = opendir(dirPath.c_str());
	if (!dir)
	{
		client->getClientFile()->openFile(dirPath.c_str(), client->getSocketFd());
		html<<"<p>Could not open directory.</p>\n";
		client->getClientResponse()->setStatusCode(404);
	}
	else
	{
		client->getClientResponse()->setStatusCode(200);
		struct dirent* entry;

		while ((entry = readdir(dir)) != NULL)
		{
			std::string name = entry->d_name;
			if (name == "." || name == "..")
                continue;
			std::string fullPath = dirPath + "/" + name;
            struct stat st;
            bool isDir = (stat(fullPath.c_str(), &st) == 0) && S_ISDIR(st.st_mode);
			if (client->getClientRequest()->get_path().rfind('/') == client->getClientRequest()->get_path().length() - 1)
				html<<"<li><a href=\"" + client->getClientRequest()->get_path() + name + "\">" + name;
			else
				html<<"<li><a href=\"" + client->getClientRequest()->get_path() + "/" + name + "\">" + name;
            if (isDir)
				html<<"/";
            html<<"</a></li>\n";
		}
		closedir(dir);
	}
	html<<"</ul>\n";
	html<<"</body>\n";
	html<<"</html>\n";
	return (html.str());
}

std::string	constructListingPath(Client *client)
{
	std::string	path = "";
	std::string	uri = client->getRouteTriggered()->getURI();
	std::string	requestPath = client->getClientRequest()->get_path();
	if (client->getRouteTriggered()->getSavedPath().empty())
		path = client->getNewPath();
	else
	{
		if (uri.find('/', 1) != std::string::npos)
			path = client->getRouteTriggered()->getRoot() + requestPath.substr(uri.length());
		else if (uri == "/")
			path = client->getRouteTriggered()->getRoot() + requestPath;
		else
		{
			if (client->getURIRequested() != uri)
			{
				if (client->getURIRequested().find('/', 1) != std::string::npos)
					path = client->getRouteTriggered()->getRoot() + requestPath.substr(client->getURIRequested().length() - 1);
				else
					path = client->getRouteTriggered()->getRoot() + requestPath.substr(client->getURIRequested().length());
			}
			else
				path = client->getRouteTriggered()->getRoot() + requestPath.substr(uri.length());
		}
	}
	client->getRouteTriggered()->setSavedPath(path);
	return (path);
}

LoadListingException::LoadListingException(int client_socket, Response *response, Client *client) :
runtime_error("Loading Listing")
{
	if (response->getBytesSent() == response->getBytesToSend() && response->getBytesToSend() != 0)
		return ;
	std::string	path = constructListingPath(client);
	std::string	body = generateListingHTML(path, client);
	if (client->getClientFile()->getFile()->is_open())
	{
		findType(client->getClientRequest(), client->getClientResponse(), client);
		if (client->getClientResponse()->getType() == "application/octet-stream")
			client->getClientResponse()->setType("text/plain");
		createHeader(client->getClientRequest(), client->getClientResponse(), client);
		return ;
	}
	printLog("ACCESS", NULL, client, client->getClientResponse(), 13, "");
	response->clearResponse();
	if (response->getStatusCode() == 200)
		response->addToResponse("HTTP/1.1 " + transformToString(response->getStatusCode()) + " OK\r\n");
	else
		response->addToResponse("HTTP/1.1 " + transformToString(response->getStatusCode()) + " Not Found\r\n");
	if (client->getClientRequest()->get_connection() == "keep-alive")
		response->addToResponse("Connection: keep-alive\r\n");
	else
		response->addToResponse("Connection: close\r\n");
	response->addToResponse("Content-Type: text/html\r\n");
	ssize_t	bodyLenght = body.size();
	response->addToResponse("Content-Length: " + transformToString(bodyLenght) + "\r\n\r\n");
	response->addToBytesToSend(response->getResponse().size() + body.size());
	sendMsgToSocket(client_socket, response->getResponse().size(), client, response);
	response->clearResponse();
	ssize_t	bytesSent = send(client_socket, body.c_str(), body.size(), MSG_NOSIGNAL);
	if (bytesSent == -1)
		throw SendException(client, response);
	response->addToBytesSent(bytesSent);
	client->setClientWritingFlag(false);
	client->setClientReadingFlag(false);
	client->setClientPending(false);
	client->getClientFile()->setReading(false);
	client->getClientFile()->setWriting(false);
	printLog("INFO", NULL, client, client->getClientResponse(), 14, "");
	if (client->getRouteTriggered()->isCgi())
	{
		client->setClientWritingFlag(true);
		client->setClientReadingFlag(true);
	}
}
