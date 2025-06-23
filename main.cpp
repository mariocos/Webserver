#include "includes/webserv.hpp"

bool	run;
bool	print;

NoPendingConnectionsException::NoPendingConnectionsException() :
runtime_error("")
{
	print = false;
}

NewConnectionCreationException::NewConnectionCreationException(Server &server, std::vector<Client*> &clientList) :
runtime_error(RED "Error adding a new client" RESET)
{
	cleaner(server, clientList, true);
}

SendException::SendException(Client *client, Response *response) :
runtime_error(RED "Error while sending" RESET)
{
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
	client->getClientFile()->setReading(false);
}

RedirectException::RedirectException(Server &server, std::vector<Client*>::iterator it) :
runtime_error(YELLOW "Redirecting Request To Default Server Block" RESET)
{
	std::vector<ServerBlock*>::iterator	serverIt = server.getDefaultServerBlock();
	(*it)->setDomainTriggered((*serverIt)->getBlockName());
	(*it)->getClientRequest()->setNewHost((*serverIt)->getBlockName());
	(*it)->setPortTriggered((*serverIt)->getBlockPort());
	(*it)->setServerBlockTriggered(*serverIt);
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
	print = false;
	close(fd);
}

BadClientException::BadClientException() :
runtime_error("BadClient")
{
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

void	printLog(std::string action, ServerBlock *serverBlock, Client *client, Response *response, int mode)
{
	switch (mode)
	{
		case 0:
			std::cout<<"["<<getTimeStamp()<<"]"<<GREEN<<" ["<<action<<"] "<<RESET;
			std::cout<<GREEN<<serverBlock->getBlockName()<<" started on port "<<serverBlock->getBlockPort()<<RESET<<std::endl;
			break;
		case 1:
			std::cout<<"["<<getTimeStamp()<<"]"<<GREEN<<" ["<<action<<"] "<<RESET;
			//set which file was used to load the ServerBlock
			std::cout<<GREEN<<"Loaded configuration from default.config"<<RESET<<std::endl;
			break;
		case 2:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<serverBlock->getBlockName()<<" shutting down."<<RESET<<std::endl;
			break;
		case 3:
			std::cout<<"["<<getTimeStamp()<<"]"<<GREEN<<" ["<<action<<"] "<<RESET;
			std::cout<<GREEN<<"New connection from " + client->getClientIp() + ":" + transformToString(client->getClientPort())<<RESET<<std::endl;
			break;
		case 4:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Disconnected "<<client->getClientIp() + ":" + transformToString(client->getClientPort())<<RESET<<std::endl;
			break;
		case 5:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<client->getClientIp() + ":" + transformToString(client->getClientPort()) + " - " + client->getClientRequest()->get_method();
			std::cout<<" "<<response->getPath()<<" "<<response->getStatusCode()<<RESET<<std::endl;
			break;
		case 6:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Served static file: "<<response->getPath()<<" (size: "<<client->getClientFile()->getFileStats()->st_size<<"KB)"<<RESET<<std::endl;
			break;
		case 7:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Executing script: "<<client->getClientRequest()->get_path()<<RESET<<std::endl;
			break;
		case 8:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Completed "<<client->getClientRequest()->get_path()<<" with status "<<response->getStatusCode()<<RESET<<std::endl;
			break;
		case 9:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<client->getClientRequest()->get_method() + " " + client->getClientRequest()->get_path() + " "<<response->getStatusCode();
			std::cout<<" from "<<client->getClientIp() + " - " + client->getClientRequest()->get_content_length() + " bytes received"<<RESET<<std::endl;
			break;
		case 10:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<client->getClientRequest()->get_method() + " " + client->getClientRequest()->get_path() + " "<<response->getStatusCode();
			std::cout<<" from "<<client->getClientIp()<<RESET<<std::endl;
			break;
		case 11:
			std::cout<<"["<<getTimeStamp()<<"]"<<YELLOW<<" ["<<action<<"] "<<RESET;
			std::cout<<YELLOW<<"Sent "<<response->getBytesSent()<<"KB of "<<client->getClientFile()->getFileStats()->st_size<<"KB"<<RESET<<std::endl;
			break;
		case 12:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"TimedOut "<<client->getClientIp() + ":" + transformToString(client->getClientPort())<<RESET<<std::endl;
			break;
		case 400:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Bad Request "<<response->getStatusCode()<<RESET<<std::endl;
			break;
		case 403:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Forbidden "<<response->getStatusCode()<<RESET<<std::endl;
			break;
		case 404:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"File Not Found "<<response->getStatusCode()<<" "<<response->getPath()<<RESET<<std::endl;
			break;
		case 405:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Method Not Allowed "<<response->getStatusCode()<<" - "<<client->getClientRequest()->get_method()<<RESET<<std::endl;
			break;
		case 409:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Conflict "<<response->getStatusCode()<<" - "<<client->getClientRequest()->get_path()<<RESET<<std::endl;
			break;
		case 413:
			std::cout<<"["<<getTimeStamp()<<"]"<<RED<<" ["<<action<<"] "<<RESET;
			std::cout<<RED<<"Payload Too Large "<<response->getStatusCode()<<" "<<response->getPath()<<RESET<<std::endl;
			break;
		case 503:
			break;
		default:
			break;
	}
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
		while (it != end)
		{
			if (*it != 0 && *it == server.getEpollIndex(i).data.fd)
			{
				std::cout<<RED "HANDLING ERROR CONNECTION" RESET<<std::endl;
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
	delete (*it);
	clientList.erase(it);
}

/* void	handlePendingConnections(std::vector<Client*> &clientList, Server &server)
{
	std::vector<Client*>::iterator	it;
	it = getPendingHole(clientList);
	if (it == clientList.end())
		throw NoPendingConnectionsException();
	while (it != clientList.end())
	{
		//checking if the Host and Port match with the ones described in the request
		if (!isConnectionGood(server, it))
			handlePortOrDomainMismatch(server, clientList, it);
		//in case the request was too big to be read in one instance
		else if (!(*it)->getClientReadingFlag() && !(*it)->getServerBlockTriggered()->isCgi())
			(*it)->readRequest((*it)->getSocketFd());
		else if ((*it)->getServerBlockTriggered()->isCgi() && (*it)->hasToSendToCgi())
		{
			//handling the CGI before and after the child process is created
			if (((*it)->getClientCgi() && !(*it)->getClientCgi()->isActive()) || !(*it)->getClientCgi())
				cgiHandler(server, (*it));
			if ((*it)->getClientWritingFlag() && (*it)->getClientReadingFlag())
				clearClient(it, clientList);
		}
		else
		{
			//handling all the other pending connections
			(*it)->handle_connect((*it)->getSocketFd());
			if ((*it)->getClientWritingFlag())
				clearClient(it, clientList);
		}
		it = getNextPendingHole(clientList, it);
	}
} */

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
			printLog("INFO", (*it)->getServerBlockTriggered(), (*it), (*it)->getClientResponse(), 12);
			(*it)->removeSocketFromEpoll((*it)->getSocketFd());
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
				print = true;
				server.handle_connections(clientList, errorFds);
				searchForTimeOut(clientList);
				//* For now this function isnt needed but keep it because idk
				//handlePendingConnections(clientList, server);
			}
			catch(const std::exception& e)
			{
				//in case of a lost child proccess after a error in execve
				if (std::string(e.what()) == "Bad child")
				{
					cleaner(server, clientList, false);
					throw BadChildException();
				}
				if (print)
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
