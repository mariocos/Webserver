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

void	new_connection(std::vector<Client*> &clientList, std::vector<int> &errorFds, Server &server)
{
	struct sockaddr_in clientaddr;
	socklen_t	addrlen = sizeof(clientaddr);
	Client	*newClient = new Client(accept(server.getServerSocket(), (sockaddr*)&clientaddr, &addrlen));
	server.addNewSocket(newClient->getClientSocket());
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
	if (setNonBlocking(newClient->getClientSocket()) == -1)
	{
        std::cerr << "Failed to set non-blocking mode." << std::endl;
        close(newClient->getClientSocket());
        throw NewConnectionCreationException(server, clientList);
    }
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
	std::vector<Client*>::iterator	it;
	it = getPendingHole(clientList);
	if (it == clientList.end())
		throw NoPendingConnectionsException();
	while (it != clientList.end())
	{
		(*it)->handle_connect((*it)->getClientSocket());
		if ((*it)->getClientWritingFlag() == true)
		{
			server.removeFromEpoll((*it)->getClientSocket());
			delete (*it);
			clientList.erase(it);
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
