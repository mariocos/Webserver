#include "includes/webserv.hpp"

bool	run;
bool	print;

NoPendingConnectionsException::NoPendingConnectionsException() :
runtime_error("")
{
	print = false;
}

NewConnectionCreationException::NewConnectionCreationException(Server &server, std::vector<Client*> &clientList) :
runtime_error(RED"Error adding a new client"RESET)
{
	cleaner(server, clientList);
}

SendException::SendException(Client *client, Response *response) :
runtime_error(RED"Error while sending"RESET)
{
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
	client->getClientFile()->setReading(false);
}

RedirectException::RedirectException(Server &server, std::vector<Client*>::iterator it) :
runtime_error(YELLOW"Redirecting Request To Default Server Block"RESET)
{
	std::vector<ServerBlock*>::iterator	serverIt = server.getDefaultServerBlock();
	(*it)->setDomainTriggered((*serverIt)->getBlockName());
	(*it)->getClientRequest()->setNewHost((*serverIt)->getBlockName());
	(*it)->setPortTriggered((*serverIt)->getBlockPort());
}

NonBlockingException::NonBlockingException(int fd) :
runtime_error(RED"Failed To Set Server Fd To Non Blocking"RESET)
{
	print = false;
	close(fd);
}

void	stopRunning(int signal)
{
	(void)signal;
	run = false;
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

void	new_connection(std::vector<Client*> &clientList, std::vector<int> &errorFds, Server &server, int serverFd)
{
	struct sockaddr_in clientaddr;
	socklen_t	addrlen = sizeof(clientaddr);
	Client	*newClient = new Client(accept(serverFd, (struct sockaddr*)&clientaddr, &addrlen));
	if (newClient->getSocketFd() == -1)
		throw NewConnectionCreationException(server, clientList);
	//still kinda hardcoded to store the information about the server block triggered
	std::vector<ServerBlock*>::iterator	it = server.getServerBlockTriggered(serverFd);
	newClient->setPortTriggered((*it)->getBlockPort());
	newClient->setDomainTriggered((*it)->getBlockName());
	newClient->addSocketToEpoll(server.getEpollFd());
	if (clientList.size() < 60)
		clientList.push_back(newClient);
	else
	{
		std::cout<<RED<<"SERVER OCUPIED"<<RESET<<std::endl;
		errorFds.push_back(newClient->getSocketFd());
		delete	newClient;
		return ;
	}
	std::cout<<"Socket Number: "<<newClient->getSocketFd()<<std::endl;
}

void	error_connection_handler(std::vector<int> &errorFds, Server &server)
{
	std::vector<int>::iterator	it = errorFds.begin();
	std::vector<int>::iterator	end = errorFds.end();
	for (int i = 0; i < server.getEpollCount(); i++)
	{
		it = errorFds.begin();
		while (it != end)
		{
			if (*it != 0 && *it == server.getEpollIndex(i).data.fd)
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
	delete (*it);
	clientList.erase(it);
}

void	handlePendingConnections(std::vector<Client*> &clientList, Server &server)
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
		else if ((*it)->getClientReadingFlag() == false)
			(*it)->readRequest((*it)->getSocketFd());
		else
		{
			(*it)->handle_connect((*it)->getSocketFd());
			if ((*it)->getClientWritingFlag() == true)
			{
				(*it)->setClientOpenFd(-1);
				(*it)->removeSocketFromEpoll((*it)->getSocketFd());
				delete (*it);
				clientList.erase(it);
			}
		}
		it = getNextPendingHole(clientList, it);
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
		//std::vector<yaml>		configs;
		std::vector<int>	ports;
		ports.push_back(4243);
		ports.push_back(8080);
		ports.push_back(3000);
		std::vector<std::string>	names;
		names.push_back("localhost");
		names.push_back("webserver.com");
		names.push_back("127.0.0.1");
		Server	server(ports, names, 10);
		std::vector<Client*>	clientList;
		std::vector<int>		errorFds;
		run = true;
		print = true;
		signal(SIGINT, stopRunning);
		while (run)
		{
			if (print)
			{
				std::cout<<"Waiting..."<<std::endl;
				print = false;
			}
			server.setEpollCount(epoll_wait(server.getEpollFd(), server.getEpollEventArray(), server.getMaxEvents(), 100));
			if (server.getEpollCount() == -1)
			{
				cleaner(server, clientList);
				return (1);
			}
			try
			{
				print = true;
				server.handle_connections(clientList, errorFds);
				handlePendingConnections(clientList, server);
			}
			catch(const std::exception& e)
			{
				if (print)
					std::cerr << e.what() << '\n';
			}
		}
		cleaner(server, clientList);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
