#include "../../../includes/Server.hpp"

Server::Server()
{
}

bool	checkIfJustDigits(const std::string &str, size_t len)
{
	size_t	n = 0;
	bool	signal = false;
	if (str[n] == '-' || str[n] == '+')
	{
		signal = true;
		n++;
	}
	while (n < len)
	{
		if (!isdigit(str[n]))
			return (false);
		n++;
	}
	return (true);
}

YamlNode* getFromYamlMap(YamlNode* node, std::string key)
{
	YamlNode* result;
	try
	{
		result = ((YamlMap*) node)->getMap().at(key);
	}
	catch(const std::exception& e)
	{
		throw ConfigFileStructureException(key);
	}
	
	return result;
}

void updateDestination(Routes *route, YamlList* modules)
{
	std::vector<YamlNode*>::iterator itModules;
	for (itModules = modules->getList().begin(); itModules != modules->getList().end(); itModules++) {
		YamlMap* module = (YamlMap*)(*itModules);
		YamlMap* settings = (YamlMap*)getFromYamlMap(module, "settings");
		std::map<std::string, YamlNode*>::iterator isDestination = settings->getMap().find("destination");
		if (isDestination != settings->getMap().end())
		{
			std::string path = ((YamlScalar<std::string>*)getFromYamlMap(settings, "destination"))->getValue();
			route->setUploadPath(path);
		}
	}
}

void routeMethods(Routes *route, YamlMap* routeConfig,YamlList* modules)
{
//	routeConfig->getMap().find("methods")->second->checkList();
		YamlList* methods = (YamlList*)getFromYamlMap(routeConfig, "methods");
	if (!routeConfig->getMap().find("methods")->second->checkList() || methods->getList().empty())
		throw ConfigFileStructureException("methods");

	std::vector<YamlNode*>::iterator itMethods;
	for (itMethods = methods->getList().begin(); itMethods != methods->getList().end(); itMethods++) {
		std::string newMeth = ((YamlScalar<std::string>*)(*itMethods))->getValue();
		if (newMeth == "GET")
			route->setMethod(GET, true);
		else if (newMeth == "POST")
		{
			route->setMethod(POST, true);
			updateDestination(route, modules);
		}
		else if (newMeth == "DELETE")
			route->setMethod(DELETE, true);
		else
			throw ConfigFileStructureException("Used method not allowed");
	}
}

bool	isRouteDefault(YamlMap* routeConfig)
{
	bool	defaultRoute = false;

	std::map<std::string, YamlNode*>::iterator isDefault = routeConfig->getMap().find("default");
	if (isDefault != routeConfig->getMap().end())
	{
		if (((YamlScalar<bool>*)(isDefault->second))->getType() == "bool")
			defaultRoute = ((YamlScalar<bool>*)getFromYamlMap(routeConfig, "default"))->getValue();
		else
			throw ConfigFileStructureException("Invalid variable type in default");
	}	
	return (defaultRoute);
}

//void	setDirListing(YamlMap* settings) {
//
//}

Routes	*setStatic(YamlMap* settings, int maxBodySize, bool defaultRoute, std::string uri)
{
	std::string root = ((YamlScalar<std::string>*)(getFromYamlMap(settings, "root")))->getValue();
	Routes* newRoute = new Routes(maxBodySize, defaultRoute, root, uri);

	try {
		std::map<std::string, YamlNode*>::iterator isDirList = settings->getMap().find("directory_listing");
		if (isDirList != settings->getMap().end()) {
//			bool dirList;
			if (((YamlScalar<bool>*)(isDirList->second))->getType() == "bool") {
//				dirList = ((YamlScalar<bool>*)(isDirList->second))->getValue();
				if (((YamlScalar<bool>*)(isDirList->second))->getValue())
					newRoute->setAsListing();
			} 
			else {
				delete newRoute;
				throw ConfigFileStructureException("Invalid variable type in directory_listing");
			}
		}
	}
	catch (const std::exception& e){
		delete newRoute;
		throw MessagelessException(e.what());
	}

	return (newRoute);
}

Routes	*setCGI(YamlMap* settings, int maxBodySize, bool defaultRoute, std::string uri)
{
	std::string root = ((YamlScalar<std::string>*)(getFromYamlMap(settings, "root")))->getValue();
	Routes* newRoute = new Routes(maxBodySize, defaultRoute, root, uri);

	try {
		YamlList* interpreters = (YamlList*)getFromYamlMap(settings, "interpreters");
		if (interpreters->getList().empty())
			throw ConfigFileStructureException("interpreters");
		std::vector<YamlNode*>::iterator itInterp;
		for (itInterp = interpreters->getList().begin(); itInterp != interpreters->getList().end(); itInterp++)
		{
			YamlMap* interpConfig = (YamlMap*)(*itInterp);
			YamlList* ext = (YamlList*)getFromYamlMap(interpConfig, "extensions");
			if (interpreters->getList().empty())
				throw ConfigFileStructureException("extensions");
			std::vector<YamlNode*>::iterator itExt;
			for (itExt = ext->getList().begin(); itExt != ext->getList().end(); itExt++) {
				std::string newExt = ((YamlScalar<std::string>*)(*itExt))->getValue();
				if (newExt == "py")
					newRoute->setCgiFileExtension(newExt);
				else
					throw MessagelessException("CGI extention not allowed");
			}
			std::string path = ((YamlScalar<std::string>*)getFromYamlMap(interpConfig, "path"))->getValue();
//			falta verificar se termina com python3 ou /python3 ou python3/
			newRoute->setUploadPath(path);
			newRoute->setAsCgi();
		}
	}
	catch (const std::exception& e){
		delete newRoute;
		throw MessagelessException(e.what());
	}
	return (newRoute);
}

Routes	*setRedirect(YamlMap* settings, int maxBodySize, bool defaultRoute, std::string uri)
{
	std::string root = "";
	Routes* newRoute = new Routes(maxBodySize, defaultRoute, root, uri);

	try {
		std::string rDirURI = ((YamlScalar<std::string>*)(getFromYamlMap(settings, "uri")))->getValue();
		std::string rDirType = ((YamlScalar<std::string>*)(getFromYamlMap(settings, "type")))->getValue();
		newRoute->setRedirectPath(rDirURI);
		
		if (rDirType == "permanent")
			newRoute->setAsPermanentRedirect();
		else if (rDirType == "temporary")
			newRoute->setAsTemporaryRedirect();
		else
			throw MessagelessException("wrong redirection type");
		
		return (newRoute);
	}
	catch (const std::exception& e){
		delete newRoute;
		throw MessagelessException(e.what());
	}
}

std::string	getUriFromYaml(YamlMap* routeConfig)
{
	std::map<std::string, YamlNode*>::iterator itUri = routeConfig->getMap().find("uri");
	if (itUri == routeConfig->getMap().end() || !getFromYamlMap(routeConfig, "uri")->checkScalar())
		throw ConfigFileStructureException("Uri not populated or wrongly populated");
	else if (((YamlScalar<int>*)(itUri->second))->getType() == "int" || ((YamlScalar<bool>*)(itUri->second))->getType() == "bool")
		throw ConfigFileStructureException("Uri not populated or wrongly populated");
	else if (((YamlScalar<std::string>*)(itUri->second))->getValue().empty())
		throw ConfigFileStructureException("Uri not populated or wrongly populated");
	else {
		std::string Uri = ((YamlScalar<std::string>*)(itUri->second))->getValue();
		if (checkIfJustDigits(Uri, Uri.length()))
			throw ConfigFileStructureException("Uri not populated or wrongly populated");
		else if (*Uri.begin() != '/')
			throw ConfigFileStructureException("Uri not populated or wrongly populated");
		else
			return (Uri);
	}
}

Routes* routeFromYaml(YamlMap* routeConfig, int maxBodySize)
{
	bool	defaultRoute = isRouteDefault(routeConfig);

	Routes* route;
	std::string uri = getUriFromYaml(routeConfig);
	YamlScalar<std::string>* type;
	YamlList* modules = (YamlList*)getFromYamlMap(routeConfig, "modules");
	if (modules->getList().empty())
		throw ConfigFileStructureException("modules is empty");
	std::vector<YamlNode*>::iterator itModules;
	for (itModules = modules->getList().begin(); itModules != modules->getList().end(); itModules++) {
		YamlMap* module = (YamlMap*)(*itModules);
		type = (YamlScalar<std::string>*)getFromYamlMap(module, "type");
//if para verificar o que esta escrito
		YamlMap* settings = (YamlMap*)getFromYamlMap(module, "settings");
		if (type->getValue().empty())
			throw ConfigFileStructureException("type");
		else if (type->getValue() == "static")
			route = setStatic(settings, maxBodySize, defaultRoute, uri);
		else if (type->getValue() == "cgi")
			route = setCGI(settings, maxBodySize, defaultRoute, uri);
		else if (type->getValue() == "redirect")
			route = setRedirect(settings, maxBodySize, defaultRoute, uri);
		else
			throw ConfigFileStructureException("type not set");
	}
	try {
		routeMethods(route, routeConfig, modules);
		return route;
	}
	catch (const std::exception& e){
		delete route;
		throw MessagelessException(e.what());
	}
}

int	maxBodySizeFromYaml(YamlMap* serverConf)
{
	std::map<std::string, YamlNode*>::iterator itMaxBodySize = serverConf->getMap().find("max_body_size");
	if (itMaxBodySize == serverConf->getMap().end())
		return -1;
	bool isScalar = itMaxBodySize->second->checkScalar();
	if (isScalar == false)
		throw ConfigFileStructureException("max_body_size not setup correctly");
	else {
//		std::cout << ((YamlScalar<int>*)(itMaxBodySize->second))->getType() << std::endl;
		if (((YamlScalar<int>*)(itMaxBodySize->second))->getType() != "int")
			throw ConfigFileStructureException("max_body_size has to be an integer");
		else {
			int maxBodySize = ((YamlScalar<int>*)getFromYamlMap(serverConf, "max_body_size"))->getValue();
//			std::cout << maxBodySize << std::endl;
			if (maxBodySize < 0)
				throw ConfigFileStructureException("max_body_size has to be equal or bigger then 0");
			else
				return maxBodySize;
		}
	}
}

void routesFromYaml(YamlMap* serverConf, std::vector<Routes*> &routes)
{
	int maxBodySize = maxBodySizeFromYaml(serverConf);
	YamlList* routesConfig = (YamlList*)getFromYamlMap(serverConf, "routes");

	if (!routesConfig->checkList() || routesConfig->getList().empty())
		throw ConfigFileStructureException("there is no route");

	try {
		std::vector<YamlNode*>::iterator itRoutes;
		for (itRoutes = routesConfig->getList().begin(); itRoutes != routesConfig->getList().end(); itRoutes++) {
			YamlMap* routeConfig = (YamlMap*)(*itRoutes);
			Routes* route = routeFromYaml(routeConfig, maxBodySize);
			// std::cout<<"IS ROUTE CGI ? "<<route->isCgi()<<std::endl;
			// std::cout<<"IS ROUTE DEFAULT ? "<<route->isDefault()<<std::endl;
			routes.push_back(route);
		}
	}
	catch (const std::exception& e){
		for (std::vector<Routes*>::iterator it = routes.begin(); it != routes.end(); ++it)
			delete *it;
		routes.clear();
		throw MessagelessException(e.what());
	}
}

void	yamlErrorPages(ServerBlock *serverBlock, YamlMap* serverConf)
{
	YamlMap *errorPagesMap = NULL;
	std::map<std::string, YamlNode*>::iterator itErrorPagesMap = serverConf->getMap().find("error_pages");
	if (itErrorPagesMap == serverConf->getMap().end())
		return;
	else if (!getFromYamlMap(serverConf, "error_pages")->checkMap())
		throw ConfigFileStructureException("error_pages not populated or wrongly populated");
	else {
		errorPagesMap = (YamlMap*)itErrorPagesMap->second;
		std::map<std::string, YamlNode*>::iterator itErrorPages;
		for (itErrorPages = errorPagesMap->getMap().begin(); itErrorPages != errorPagesMap->getMap().end(); itErrorPages++) {
			if (!checkIfJustDigits(itErrorPages->first, itErrorPages->first.length()))
				throw ConfigFileStructureException("error_pages has to be a number between 400 and 499");
			int errorNbr = atoi(itErrorPages->first.c_str());
			std::string page = ((YamlScalar<std::string>*)itErrorPages->second)->getValue();
			if (errorNbr < 400 || errorNbr > 499)
				throw ConfigFileStructureException("error_pages has to be a number between 400 and 499");
			if (page.empty())
				throw ConfigFileStructureException("error_pages not defined");
			serverBlock->setErrorPage(errorNbr, page);
		}
	}
}

int	getPortFromYaml(YamlMap* serverConf)
{
	std::map<std::string, YamlNode*>::iterator itListen = serverConf->getMap().find("listen");
	if (itListen == serverConf->getMap().end())
		throw ConfigFileStructureException("'listen: <port>' is mandatory");
	bool isScalar = itListen->second->checkScalar();
	if (isScalar == false)
		throw ConfigFileStructureException("listen not setup correctly");
	else {
//		std::cout << ((YamlScalar<int>*)(itListen->second))->getType() << std::endl;
		if (((YamlScalar<int>*)(itListen->second))->getType() != "int")
			throw ConfigFileStructureException("listen has to be between 0 - 65535");
		else {
			int port = ((YamlScalar<int>*)getFromYamlMap(serverConf, "listen"))->getValue();
//			std::cout << port << std::endl;
			if (port >= 0 && port <= 65535)
				return port;
			else
				throw ConfigFileStructureException("listen has to be between 0 - 65535");
		}
	}
}

std::string	getDomainNameFromYaml(YamlMap* serverConf)
{
	std::map<std::string, YamlNode*>::iterator itServerName = serverConf->getMap().find("server_names");
	if(!itServerName->second->checkScalar())
		throw ConfigFileStructureException("server_names not setup correctly");
	else {
//		std::cout << ((YamlScalar<int>*)(itServerName->second))->getType() << std::endl;
		if (((YamlScalar<int>*)(itServerName->second))->getType() == "int" || ((YamlScalar<bool>*)(itServerName->second))->getType() == "bool")
			throw ConfigFileStructureException("server_names has to be a string");
		else if (((YamlScalar<std::string>*)(itServerName->second))->getValue().empty())
			throw ConfigFileStructureException("server_names can't be empty");
		else {
			std::string str = ((YamlScalar<std::string>*)(itServerName->second))->getValue();
			if (checkIfJustDigits(str, str.length()))
				throw ConfigFileStructureException("server_names can't be just numbers");
			else
				return str;
		}
	}
}

ServerBlock* serverBlockFromYaml(YamlMap* serverConf)
{
	int port = getPortFromYaml(serverConf);
//	((YamlScalar<int>*)(getFromYamlMap(serverConf, "listen")))->getValue();
	int backlog = -1;
	std::string domainName = getDomainNameFromYaml(serverConf);
//	((YamlScalar<std::string>*)(getFromYamlMap(serverConf, "server_names")))->getValue();
	bool Default = isRouteDefault(serverConf);
//	std::cout << "Is default: " << Default << std::endl;
	std::vector<Routes*>	tmp;
	ServerBlock* newServerBlock = NULL;

	if (port == 0)
		throw ConfigFileStructureException("listen");
	if (domainName.empty())
		throw ConfigFileStructureException("server_names");

	try {
		int serverSock = socket(AF_INET, SOCK_STREAM, 0);
		newServerBlock = new ServerBlock(serverSock, port, backlog, domainName, Default);
		
		routesFromYaml(serverConf, tmp);
//		possivelmente aqui é onde tenho de verificar quantos serveres tenho na mesma porta
		newServerBlock->setBlockRoutes(tmp);

		yamlErrorPages(newServerBlock, serverConf);
	}
	catch (const std::exception& e){
		delete newServerBlock;
		throw MessagelessException(e.what());
	}
//	TODO set the error pages defined in the config file
//	newServerBlock->setErrorPage(400, "website/400.html");
//	newServerBlock->setErrorPage(403, "website/403.html");
//	newServerBlock->setErrorPage(404, "website/404.html");
//	newServerBlock->setErrorPage(405, "website/405.html");
//	newServerBlock->setErrorPage(413, "website/413.html");

	return newServerBlock;
}

void Server::startServerBlock(ServerBlock* newServerBlock)
{
	int	option = 1;
	struct sockaddr_in servaddr;
	struct epoll_event	event;

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	std::cout<<"PORT: "<<newServerBlock->getBlockPort()<<std::endl;
	servaddr.sin_port = htons(newServerBlock->getBlockPort());
	if (setsockopt(newServerBlock->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1 \
		|| bind(newServerBlock->getSocketFd(), (const sockaddr *)&servaddr, sizeof(servaddr)) == -1 \
		|| listen(newServerBlock->getSocketFd(), newServerBlock->getBlockMaxConnections()) == -1)
	{
		delete newServerBlock;
		close(this->_epoll_fd);
		throw SocketBindException(*this);
	}
	event.events = EPOLLIN;
	event.data.fd = newServerBlock->getSocketFd();
	if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
	{
		delete newServerBlock;
		close(this->_epoll_fd);
		throw EpollCtlException(*this);
	}
}

void checkDefaultServerBlock(std::vector<ServerBlock*> &_serverBlocks)
{
	size_t	defaultServerBlockNbr = 0;
	size_t	defaultRouteNbr = 0;

	for (std::vector<ServerBlock*>::iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); it++)
	{
		std::vector<Routes*> routes = (*it)->getRoutesVector();
		for (std::vector<Routes*>::iterator it = routes.begin(); it != routes.end(); it++)
		{
			if ((*it)->isDefault() == true)
				defaultRouteNbr++;
		}
		if (defaultRouteNbr > 1)
			throw ConfigFileStructureException("too many default routes in one blocks, only one allowed per server block");
		if (defaultRouteNbr == 0)
			(*it)->getRoutesVector().front()->setAsDefaultRoute();
		defaultRouteNbr = 0;
		if ((*it)->isDefault() == true)
			defaultServerBlockNbr++;
	}
	if (defaultServerBlockNbr > 1)
		throw ConfigFileStructureException("too many default server blocks, only one allowed");
	if (defaultServerBlockNbr == 0)
		_serverBlocks.front()->setAsDefault();
}

Server::Server(YamlNode *parsedConf) : _maxEvents(10)
{
	ServerBlock			*newServerBlock;

	this->_epoll_fd = epoll_create(1);
	if (this->_epoll_fd == -1)
		throw EpollCreationException();

	YamlList* serverConfList = (YamlList*)getFromYamlMap(parsedConf, "servers");
//	if (parsedConf->checkMap())
//		throw ConfigFileStructureException("'-'");
	if (parsedConf->checkList() && serverConfList->getList().empty())
		throw ConfigFileStructureException("servers");
	try {
		std::vector<YamlNode*>::iterator it;
		for (it = serverConfList->getList().begin(); it != serverConfList->getList().end(); it++) {
			YamlMap* serverConf = (YamlMap*)(*it);
			newServerBlock = serverBlockFromYaml(serverConf);
			// std::cout<<"IS SERVER BLOCK DEFAULT ? "<<newServerBlock->isDefault()<<std::endl;
			startServerBlock(newServerBlock);
			this->_serverBlocks.push_back(newServerBlock);
			checkDefaultServerBlock(this->_serverBlocks);
			printLog("INFO", newServerBlock, NULL, NULL, 0, "");
		}
	}
	catch(const std::exception& e)
	{
		delete parsedConf;
//		delete newServerBlock;
		close(this->_epoll_fd);
		for (std::vector<ServerBlock*>::iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it) 
			delete *it;
		throw MessagelessException(e.what());
	}
	this->_events = new epoll_event[this->_maxEvents];
}

Server::Server(std::vector<int> ports, std::vector<std::string> names, int backlog) : _maxEvents(backlog)
{
	int	option = 1;
	struct sockaddr_in servaddr;
	struct epoll_event	event;
	ServerBlock	*newServerBlock;
	Routes		*newRoute;

	this->_epoll_fd = epoll_create(1);
	if (this->_epoll_fd == -1)
		throw EpollCreationException();

	for (size_t i = 0; i < ports.size(); i++)
	{
		std::vector<Routes*>	tmp;
		if (names[i] == "localhost")
			newServerBlock = new ServerBlock(socket(AF_INET, SOCK_STREAM, 0), ports[i], -1, names[i], true);
		else
			newServerBlock = new ServerBlock(socket(AF_INET, SOCK_STREAM, 0), ports[i], -1, names[i], false);
		if (newServerBlock->getSocketFd() == -1)
		{
			delete newServerBlock;
			close(this->_epoll_fd);
			throw SocketCreationException(*this);
		}
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(newServerBlock->getBlockPort());
		if (setsockopt(newServerBlock->getSocketFd(), SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1 \
			|| bind(newServerBlock->getSocketFd(), (const sockaddr *)&servaddr, sizeof(servaddr)) == -1 \
			|| listen(newServerBlock->getSocketFd(), newServerBlock->getBlockMaxConnections()) == -1)
		{
			delete newServerBlock;
			close(this->_epoll_fd);
			throw SocketBindException(*this);
		}
		event.events = EPOLLIN;
		event.data.fd = newServerBlock->getSocketFd();
		if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event) == -1)
		{
			delete newServerBlock;
			close(this->_epoll_fd);
			throw EpollCtlException(*this);
		}

		// TODO set the error pages defined in the config file
		newServerBlock->setErrorPage(400, "website/400.html");
		newServerBlock->setErrorPage(403, "website/403.html");
		newServerBlock->setErrorPage(404, "website/404.html");
		newServerBlock->setErrorPage(405, "website/405.html");
		newServerBlock->setErrorPage(413, "website/413.html");

		// TODO implement this as long as the routes list size
		// for (size_t n = 0; i < list.size(); n++)
		// {
		// 		newRoute = new Routes(-1, false, "/", "/");
		// 		tmp.push_back(newRoute);
		// }
		// newServerBlock->setBlockRoutes(tmp);
		// this->_serverBlocks.push_back(newServerBlock);

		// PUT THIS inside the for loop when i get the route list size
		for (size_t n = 0; n < 5; n++)
		{
			if (n == 0)
			{
				newRoute = new Routes(-1, true, "website", "/");
				newRoute->setMethod(GET, true);
				newRoute->setMethod(POST, true);
				newRoute->setMethod(DELETE, true);
				std::string	file = "dummy.html";
				std::string	uploadPath = "./upload";
				newRoute->setDefaultFileForDirectory(file);
				newRoute->setUploadPath(uploadPath);
			}
			else if (n == 1)
			{
				newRoute = new Routes(-1, false, "website/cgi-bin", "/find-this");
				newRoute->setMethod(GET, true);
				newRoute->setMethod(POST, true);
				newRoute->setMethod(DELETE, false);
				std::string	extension = "py";
				std::string	uploadPath = "./upload";
				newRoute->setCgiFileExtension(extension);
				newRoute->setUploadPath(uploadPath);
			}
			else if (n == 2)
			{
				newRoute = new Routes(-1, false, "/home/pauberna/Desktop/projetos_42", "/projects/");
				newRoute->setAsListing();
				newRoute->setMethod(GET, true);
			}
			else if (n == 3)
			{
				newRoute = new Routes(-1, false, "website", "/google");
				newRoute->setAsPermanentRedirect();
				std::string path = "https://google.com";
				newRoute->setRedirectPath(path);
				newRoute->setMethod(GET, true);
			}
			else
			{
				newRoute = new Routes(-1, false, "website", "/redirect");
				newRoute->setAsTemporaryRedirect();
				std::string path = "https://youtube.com";
				newRoute->setRedirectPath(path);
				newRoute->setMethod(GET, true);
			}
			if (ports[i] == 2424 && n != 0)
				newRoute->setAsCgi();
			tmp.push_back(newRoute);
			if (newRoute->canDoMethod(POST) && !newRoute->isCgi() && newRoute->getUploadPath().empty())
			{
				delete newServerBlock;
				throw NoUploadPathException(*this, tmp);
			}
		}
		newServerBlock->setBlockRoutes(tmp);
		printLog("INFO", newServerBlock, NULL, NULL, 0, "");
		printLog("INFO", newServerBlock, NULL, NULL, 1, "");
		this->_serverBlocks.push_back(newServerBlock);
	}
	this->_events = new epoll_event[this->_maxEvents];
}

Server::~Server()
{
	if (this->_events)
		delete[] this->_events;
	close(this->_epoll_fd);
	for (std::vector<ServerBlock*>::iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it)
		delete *it;
	_serverBlocks.clear();
}

void	Server::setEpollCount(int count)
{
	this->_epoll_count = count;
}

int	Server::getEpollFd()
{
	return (this->_epoll_fd);
}

int	Server::getEpollCount()
{
	return (this->_epoll_count);
}

int	Server::getMaxEvents()
{
	return (this->_maxEvents);
}

int	Server::getServerSocketTriggered(int fd)
{
	std::vector<ServerBlock*>::iterator	it = this->_serverBlocks.begin();
	while (it != this->_serverBlocks.end())
	{
		if (fd == (*it)->getSocketFd())
			return (fd);
		it++;
	}
	return (-1);
}

std::vector<Routes*>::iterator	Server::getRouteTriggered(std::string uri, int fd)
{
	std::vector<ServerBlock*>::iterator	it = this->getServerBlockTriggered(fd);
	std::vector<Routes*>::iterator	routeIt = (*it)->getRoutesVector().begin();
	while (routeIt != (*it)->getRoutesVector().end())
	{
		if ((*routeIt)->getURI().rfind('/') == (*routeIt)->getURI().length() - 1 && uri == (*routeIt)->getURI())
			return (routeIt);
		else if ((*routeIt)->getURI().rfind('/') != (*routeIt)->getURI().length() - 1)
		{
			if (uri == (*routeIt)->getURI() || uri == (*routeIt)->getURI() + "/")
				return (routeIt);
		}
		routeIt++;
	}
	return (this->getDefaultRoute(fd));
}

std::vector<Routes*>::iterator	Server::getDefaultRoute(int fd)
{
	std::vector<ServerBlock*>::iterator	it = this->getServerBlockTriggered(fd);
	if (it != this->getServerBlocks().end())
	{
		std::vector<Routes*>::iterator	routeIt = (*it)->getRoutesVector().begin();
		while (routeIt != (*it)->getRoutesVector().end())
		{
			if ((*routeIt)->isDefault())
				return (routeIt);
			routeIt++;
		}
		return ((*it)->getRoutesVector().end());
	}
	return ((*this->getDefaultServerBlock())->getDefaultRoute());
}

std::vector<ServerBlock*>::iterator	Server::getServerBlockTriggered(int fd)
{
	std::vector<ServerBlock*>::iterator	it = this->_serverBlocks.begin();
	while (it != this->_serverBlocks.end())
	{
		if (fd == (*it)->getSocketFd())
			return (it);
		it++;
	}
	return (this->_serverBlocks.end());
}

std::vector<ServerBlock*>::iterator	Server::getDefaultServerBlock()
{
	std::vector<ServerBlock*>::iterator	it = this->_serverBlocks.begin();
	while (it != this->_serverBlocks.end())
	{
		if ((*it)->isDefault())
			return (it);
		it++;
	}
	return (this->_serverBlocks.end());
}

std::vector<ServerBlock*>	Server::getServerBlocks()
{
	return (this->_serverBlocks);
}

ServerBlock	Server::getServerBlock(int index)
{
	return (*this->_serverBlocks[index]);
}

std::vector<Client*>	&Server::getClientListVector()
{
	return (this->_clientList);
}

std::vector<int>	&Server::getErrorFdsVector()
{
	return (this->_errorFds);
}

epoll_event	*Server::getEpollEventArray()
{
	return (this->_events);
}

epoll_event	&Server::getEpollIndex(int index)
{
	return (this->_events[index]);
}

void	Server::removeFromEpoll(int fd)
{
	epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void	Server::handle_connections()
{
	//handling all the triggers made in epoll_wait
	for (int i = 0; i < this->_epoll_count; i++)
	{
		error_connection_handler(*this);
		struct epoll_event	&event = this->_events[i];
		if (this->getServerSocketTriggered(event.data.fd) != -1)
		{
			try
			{
				new_connection(*this, this->getServerSocketTriggered(event.data.fd));
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		else
			this->manageConnection(event);
	}
}

void	Server::manageConnection(epoll_event &event)
{
	std::vector<Client*>::iterator	it;

	it = isThisPipe(event.data.fd, *this);
	if (it != this->_clientList.end())
	{
		try
		{
			//in case the fd triggered is from a pipe
			cgiHandler(*this, (*it), event.data.fd);
			return;
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) == "Bad child")
				throw BadChildException();
			if (std::string(e.what()) != "BadClient")
				std::cerr << e.what() << '\n';
			clearClient(it, *this);
		}
	}
	//getting from the clientList which client was triggered/disconnected
	it = getRightHole(*this, event.data.fd);
	if (it == this->_clientList.end())
		return;
	else if ((event.events == EPOLLRDHUP) && \
		(*it)->getClientReadingFlag() && (*it)->getClientWritingFlag() && (*it)->getClientOpenFd() == -1)
		clearClient(it, *this);
	else if (!(*it)->getClientReadingFlag())
	{
		try
		{
			if (event.events == EPOLLIN && !(*it)->getClientPending())
			{
				(*it)->readRequest((*it)->getSocketFd());
				if ((*it)->getClientReadingFlag())
					(*it)->setClientWritingFlag(false);
				(*it)->resetTimer();
				(*it)->setClientPending(true);
				(*it)->setSocketToWriting(this->_epoll_fd);
				(*it)->setRouteTriggered((*this->getRouteTriggered((*it)->getURIRequested(), (*it)->getSocketTriggered())));
			}
			else if (event.events == EPOLLOUT)
			{
				//in case the body is still to be sent
				send((*it)->getSocketFd(), "HTTP/1.1 100 Continue\r\n\r\n", 25, MSG_NOSIGNAL);
				(*it)->setSocketToReading(this->_epoll_fd);
			}
			else if ((*it)->getClientPending())
				(*it)->readBodyOfRequest(*this, (*it)->getClientRequest());
			
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) == "Expectation" || std::string(e.what()) == "HTTPVersion")
			{
				if ((*it)->getClientRequest()->get_connection() == "keep-alive")
					(*it)->resetClient(*this);
				else
					clearClient(it, *this);
			}
			else if (std::string(e.what()) != "EmptyBuffer")
				std::cerr << e.what() << '\n';
		}
	}
	else
		this->manageClient(it, event);
}

void	Server::manageClient(std::vector<Client*>::iterator it, epoll_event	&event)
{
	if (!isConnectionGood(*this, it))
	{
		try
		{
			handlePortOrDomainMismatch(*this, it);	
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			if ((*it)->getClientRequest()->get_connection() == "keep-alive")
				(*it)->resetClient(*this);
			else
				clearClient(it, *this);
		}
	}
	else if ((*it)->getRouteTriggered()->isCgi())
	{
		try
		{
			//handling the CGI before and after the child process is created
			if ((*it)->getClientCgi() && (*it)->getClientCgi()->isActive())
				return ;
			cgiHandler(*this, (*it), event.data.fd);
			if ((*it)->getClientWritingFlag() && (*it)->getClientReadingFlag())
			{
				(*it)->resetClient(*this);
				delete (*it)->getClientCgi();
				(*it)->setClientCgi(NULL);
			}
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) == "Bad child")
				throw BadChildException();
			if (std::string(e.what()) != "BadClient")
				std::cerr << e.what() << '\n';
			clearClient(it, *this);
		}
	}
	else
	{
		try
		{
			if (event.events == EPOLLOUT)
				(*it)->getClientRequest()->execute_response((*it)->getSocketFd(), (*it));
			else
				return ;
			if (!(*it)->getClientFile()->isReading() && !(*it)->getClientFile()->isWriting())
				(*it)->resetClient(*this);
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) == "Redirect")
			{
				if ((*it)->getClientRequest()->get_connection() == "keep-alive")
					(*it)->resetClient(*this);
				else
					clearClient(it, *this);
			}
			else if (std::string(e.what()) != "Loading Listing")
			{
				std::cerr << e.what() << '\n';
				clearClient(it, *this);
			}
			else if (!(*it)->getClientFile()->getFile()->is_open())
				(*it)->resetClient(*this);
		}
	}
}

void	Server::addClientToClientVector(Client *client)
{
	this->_clientList.push_back(client);
}

void	Server::removeClientFromClientVector(std::vector<Client*>::iterator client)
{
	this->_clientList.erase(client);
}

void	Server::addErrorFdToErrorVector(int fd)
{
	this->_errorFds.push_back(fd);
}

void	Server::removeErrorFdFromErrorVector(std::vector<int>::iterator errorFd)
{
	this->_errorFds.erase(errorFd);
}

Server::SocketCreationException::SocketCreationException(Server &server) :
runtime_error(RED"Error creating the socket"RESET)
{
	cleaner(server, true);
}

Server::SocketBindException::SocketBindException(Server &server) :
runtime_error(RED"Error binding the socket"RESET)
{
	cleaner(server, true);
}

Server::EpollCreationException::EpollCreationException() :
runtime_error(RED"Error creating the epoll_fd"RESET) {}

Server::EpollCtlException::EpollCtlException(Server &server) :
runtime_error(RED"Error managing the epoll"RESET)
{
	cleaner(server, true);
}

Server::NoFileToReadException::NoFileToReadException() :
runtime_error(RED"No File Pending"RESET) {}
