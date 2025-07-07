#include "includes/webserv.hpp"

bool	run;

NoPendingConnectionsException::NoPendingConnectionsException() :
runtime_error("")
{
}

NewConnectionCreationException::NewConnectionCreationException(Server &server, std::vector<Client*> &clientList) :
runtime_error(RED "Error adding a new client" RESET)
{
	cleaner(server, clientList, true);
}

SendException::SendException(Client *client, Response *response) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] Error while sending to " + client->getClientIp() + ":" + transformToString(client->getClientPort()) + RESET)
{
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
	client->getClientFile()->setReading(false);
}

ReadException::ReadException(Client *client, Response *response) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] Error while reading from " + client->getClientIp() + ":" + transformToString(client->getClientPort()) + RESET)
{
	response->clearResponse();
	client->setClientReadingFlag(true);
	client->setClientPending(false);
	client->getClientFile()->setWriting(false);
}

RedirectException::RedirectException(Server &server, std::vector<Client*>::iterator it) :
runtime_error("[" + getTimeStamp() + "]" + YELLOW + " [INFO] Redirecting Request from " + (*it)->getClientIp() + ":" + transformToString((*it)->getClientPort()) + " to Default Server Block" + RESET)
{
	std::vector<ServerBlock*>	serverBlocks = server.getServerBlocks();
	std::vector<ServerBlock*>::iterator	serverIt = serverBlocks.begin();
	while (serverIt != serverBlocks.end())
	{
		if ((*serverIt)->isDefault())
			break;
		serverIt++;
	}
	(*it)->setDomainTriggered((*serverIt)->getBlockName());
	(*it)->getClientRequest()->setNewHost((*serverIt)->getBlockName());
	(*it)->setPortTriggered((*serverIt)->getBlockPort());
	(*it)->setServerBlockTriggered(*serverIt);
	(*it)->setRouteTriggered((*(*serverIt)->getDefaultRoute()));
}

BadChildException::BadChildException() :
runtime_error("Bad child")
{
}

BadPipeCreationException::BadPipeCreationException() :
runtime_error(RED "Error opening the pipe" RESET) {}

NonBlockingException::NonBlockingException(int fd) :
runtime_error(RED "Failed To Set Server Fd To Non Blocking" RESET)
{
	close(fd);
}

BadClientException::BadClientException() :
runtime_error("BadClient")
{
}

EmptyBufferException::EmptyBufferException() :
runtime_error("EmptyBuffer")
{
}

NoUploadPathException::NoUploadPathException(Server &server, std::vector<Routes*> tmp) :
runtime_error(RED "No Upload Path setted for a POST route" RESET)
{
	close(server.getEpollFd());
	std::vector<Routes*>::iterator	it = tmp.begin();
	while (it != tmp.end())
	{
		delete (*it);
		it++;
	}
	cleanerForServerCreation(server, true);
}

std::string	getTimeStamp()
{
	time_t	now = time(NULL);
	struct tm *timeStruct = localtime(&now);
	char	buffer[20];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeStruct);
	return (std::string(buffer));
}

std::string	convertIpToString(struct in_addr s_addr)
{
	uint32_t	addr = ntohl(s_addr.s_addr);
	std::ostringstream	ostream;
	std::string	output;

	for (int i = 3; i >= 0; i--)
	{
		ostream << ((addr >> (i * 8)) & 0xFF);
		if (i > 0)
			ostream << '.';
	}
	output = ostream.str();
	return (output);
}

bool	checkInt(const std::string &str, size_t len)
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
			break ;
		n++;
	}
	if (n == len && ((signal == true && len < 11) || (signal == false && len < 10)))
		return (true);
	return (false);
}
/*
	0 -> Bool
	1 -> Int
	2 -> YamlNode
	3 -> std::string
 */
int		returnVariableType(std::string &value)
{
	if (value == "true" || value == "false" || value == "on" || value == "off" || value == "yes" || value == "no")
		return (0);
	else if (checkInt(value, value.length()))
		return (1);
	else if ("")
		return (2);
	else
		return (3);
}

void	stopRunning(int signal)
{
	(void)signal;
	run = false;
	std::cout<<std::endl;
}

void	ft_bzero(void *s, size_t n)
{
	char	*p;

	p = reinterpret_cast<char*>(s);
	while (n != 0)
	{
		*p = 0;
		p++;
		n--;
	}
}

void	error_connection_handler(std::vector<int> &errorFds, Server &server)
{
	std::vector<int>::iterator	it = errorFds.begin();
	std::vector<int>::iterator	end = errorFds.end();
	if (it == end)
		return ;
	for (int i = 0; i < server.getEpollCount(); i++)
	{
		it = errorFds.begin();
		epoll_event event = server.getEpollIndex(i);
		while (it != errorFds.end())
		{
			if (*it == event.data.fd && ((event.events & EPOLLIN) || (event.events & EPOLLOUT)))
			{
				loadError503((*it));
				server.removeFromEpoll((*it));
				errorFds.erase(it);
				break;
			}
			it++;
		}	
	}
}

bool	isConnectionGood(Server &server, std::vector<Client*>::iterator it)
{
	std::vector<ServerBlock*>	copy = server.getServerBlocks();
	std::vector<ServerBlock*>::iterator	serverIt = copy.begin();
	while (serverIt != copy.end())
	{
		if ((*it)->getPortTriggered() == (*serverIt)->getBlockPort())
		{
			if ((*it)->getClientRequest()->get_host() == (*serverIt)->getBlockName())
				return (true);
		}
		serverIt++;
	}
	return (false);
}

bool	doesPortsMatch(Server &server, std::vector<Client*>::iterator it)
{
	std::vector<ServerBlock*>	copy = server.getServerBlocks();
	std::vector<ServerBlock*>::iterator	serverIt = copy.begin();
	while (serverIt != copy.end())
	{
		if ((*it)->getPortTriggered() == (*serverIt)->getBlockPort())
			return (true);
		serverIt++;
	}
	return (false);
}

void	handlePortOrDomainMismatch(Server &server, std::vector<Client*> &clientList, std::vector<Client*>::iterator it)
{
	if (doesPortsMatch(server, it))
		throw RedirectException(server, it);
	(*it)->setClientOpenFd(-1);
	(*it)->removeSocketFromEpoll((*it)->getSocketFd());
	(*it)->getServerBlockTriggered()->decreaseConnections();
	delete (*it);
	clientList.erase(it);
}

void	searchForTimeOut(std::vector<Client*> &clientList)
{
	std::vector<Client*>::iterator	it = clientList.begin();
	if (it == clientList.end())
		return ;
	while (it != clientList.end())
	{
		if ((*it)->hasTimedOut())
		{
			loadError408((*it)->getSocketFd(), (*it)->getClientResponse(), (*it));
			printLog("INFO", NULL, (*it), (*it)->getClientResponse(), 12);
			(*it)->removeSocketFromEpoll((*it)->getSocketFd());
			close((*it)->getSocketFd());
			(*it)->getServerBlockTriggered()->decreaseConnections();
			delete (*it);
			clientList.erase(it);
			it = clientList.begin();
			if (it == clientList.end())
				return ;
		}
		it++;
	}
}

int	main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cout<<RED<<"Wrong number of arguments"<<RESET<<std::endl;
		return (1);
	}
	std::string	config("default.config");
	if (ac == 2)
		config = av[1];
	try
	{
		std::vector<int>	ports;
		ports.push_back(4243);
		ports.push_back(8080);
		ports.push_back(3000);
		ports.push_back(2424);
		std::vector<std::string>	names;
		names.push_back("localhost");
		names.push_back("webserver.com");
		names.push_back("127.0.0.1");
		names.push_back("script");
		Server	server(ports, names, 20);
		std::vector<Client*>	clientList;
		std::vector<int>		errorFds;
		run = true;
		signal(SIGINT, stopRunning);
		while (run)
		{
			server.setEpollCount(epoll_wait(server.getEpollFd(), server.getEpollEventArray(), server.getMaxEvents(), 100));
			if (server.getEpollCount() == -1)
			{
				cleaner(server, clientList, true);
				return (1);
			}
			try
			{
				server.handle_connections(clientList, errorFds);
				searchForTimeOut(clientList);
			}
			catch(const std::exception& e)
			{
				//in case of a lost child proccess after a error in execve
				if (std::string(e.what()) == "Bad child")
				{
					cleaner(server, clientList, false);
					throw BadChildException();
				}
				std::cerr << e.what() << '\n';
			}
		}
		cleaner(server, clientList, true);
	}
	catch(const std::exception& e)
	{
		if (std::string(e.what()) == "Bad child")
			return (0);
		std::cerr << e.what() << '\n';
	}
	return (0);
}

//command to test GET to another serverBlock besides the localhost : "curl -H "Host: webserver.com" http://localhost:8080"
//command to test POST : "curl -X POST -d "whatever we want to send as content" -H "Host: webserver.com" http://localhost:8080/chicken-jokey"
//command to test POST with binary : "curl -X POST --data-binary @/bin/ls -H "Host: webserver.com" http://localhost:8080/chicken-jokey"
//command to test GET to CGI : "curl -H "Host: script" http://localhost:2424/cgi-bin/hello.py\?name\=Jonh\&age\=30"
//command to test POST to CGI : "curl -X POST -H "Host: script" -d "name=paul&lang=python" http://localhost:2424/cgi-bin/hello.py\?name\=Jonh\&age\=30"
//command to test 1000 requests with siege : "siege -b -c 50 -r 20 http://localhost:4243/index.html"
//command to test 1000 requests with siege : "siege -b -c 100 -r 10 http://localhost:4243/index.html"
//command to test 1000 requests with siege : "siege -b -c 200 -r 5 http://localhost:4243/index.html"
