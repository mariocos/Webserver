#include "webserv.hpp"

bool	run;

NoPendingConnectionsException::NoPendingConnectionsException() :
runtime_error("Nothing Pending") {}

NewConnectionCreationException::NewConnectionCreationException(Server &server, std::vector<Client*> &clientList) :
runtime_error("Error adding a new client")
{
	cleaner(server, clientList);
}

void	stopRunning(int signal)
{
	(void)signal;
	run = false;
}

void	check(int algo)
{
	if (algo == -1)
	{
		perror("");
		std::cerr<<RED<<"Error msg"<<RESET<<std::endl;
		exit(1);
	}
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


//trying to close connections after a timeout of inactivity
void	searchDeadConnections(std::vector<Client*> &clientList, Server &server)
{
	std::vector<Client*>::iterator	it = clientList.begin();
	std::vector<Client*>::iterator	end = clientList.end();
	while (it != end)
	{
		if (*it != NULL && (*it)->connectionExpired(5) == true)
		{
			server.removeFromEpoll((*it)->getClientSocket());
			close((*it)->getClientSocket());
			delete (*it);
			clientList.erase(it);
			std::cout<<RED<<"Connection Expired"<<RESET<<std::endl;
			it = clientList.begin();
			end = clientList.end();
		}
		else
			it++;
	}
}

void	new_connection(std::vector<Client*> &clientList, std::vector<int> &errorFds, Server &server)
{
	struct sockaddr_in clientaddr;
	socklen_t	addrlen = sizeof(clientaddr);
	Client	*newClient = new Client(accept(server.getServerSocket(), (struct sockaddr*)&clientaddr, &addrlen));
	if (newClient->getClientSocket() == -1)
		throw NewConnectionCreationException(server, clientList);
	server.addNewSocket(newClient->getClientSocket());
	newClient->setStartingTime();
	if (clientList.size() < 60)
		clientList.push_back(newClient);
	else
	{
		std::cout<<RED<<"SERVER OCUPIED"<<RESET<<std::endl;
		errorFds.push_back(newClient->getClientSocket());
		delete	newClient;
		return ;
	}
	std::cout<<"Socket Number: "<<newClient->getClientSocket()<<std::endl;
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

void	handlePendingConnections(std::vector<Client*> &clientList, Server &server)
{
	(void)server;
	std::vector<Client*>::iterator	it;
	it = getPendingHole(clientList);
	if (it == clientList.end())
		throw NoPendingConnectionsException();
	while (it != clientList.end())
	{
		if ((*it)->getClientReadingFlag() == false)
			(*it)->readRequest((*it)->getClientSocket());
		else
		{
			(*it)->handle_connect((*it)->getClientSocket());
			if ((*it)->getClientWritingFlag() == true)
			{
				close((*it)->getClientSocket());
				(*it)->setClientOpenFd(-1);
				server.removeFromEpoll((*it)->getClientSocket());
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
		Server	server(4243, 10);
		std::vector<Client*>	clientList;
		std::vector<int>		errorFds;
		run = true;
		signal(SIGINT, stopRunning);
		while (run)
		{
			std::cout<<"Waiting..."<<std::endl;
			server.setEpollCount(epoll_wait(server.getEpollFd(), server.getEpollEventArray(), server.getMaxEvents(), 100));
			if (server.getEpollCount() == -1)
			{
				cleaner(server, clientList);
				return (1);
			}
			try
			{
				//this still does nothing because the socket is closed after its use
				searchDeadConnections(clientList, server);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
			try
			{
				handlePendingConnections(clientList, server);
			}
			catch(const std::exception& e)
			{
				std::cerr << RED << e.what() << RESET << '\n';
			}
			try
			{
				server.handle_connections(clientList, errorFds);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				return (1);
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
