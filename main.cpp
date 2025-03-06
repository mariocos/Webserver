#include "webserv.hpp"

void	check(int algo)
{
	if (algo == -1)
	{
		perror("");
		std::cerr<<RED<<"Error msg"<<RESET<<std::endl;
		exit(1);
	}
}

int	setup(short port, int backlog)
{
	int server_socket;
	int	option = 1;
	struct sockaddr_in servaddr;

	check((server_socket = socket(AF_INET, SOCK_STREAM, 0)));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option, 4);
	check(bind(server_socket, (const sockaddr *)&servaddr, sizeof(servaddr)));
	check(listen(server_socket, backlog));
	return (server_socket);
}

Client	*new_connection(int server_socket, int epoll_fd)
{
	struct sockaddr_in clientaddr;
	struct epoll_event	event;
	socklen_t	addrlen = sizeof(clientaddr);
	Client	*newClient = new Client(accept(server_socket, (sockaddr*)&clientaddr, &addrlen));
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = newClient->getClientSocket();
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event);
	std::cout<<"Socket Number: "<<newClient->getClientSocket()<<std::endl;
	return (newClient);
}

bool	handle_connect(int client_socket, Client *client)
{
	char	buffer[4096];
	ssize_t	bytes_read;

	bzero(buffer, sizeof(buffer));
	bytes_read = read(client_socket, buffer, sizeof(buffer));
	if (bytes_read == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return (true);
		else
		{
			perror("read: ");
			close(client_socket);
		}
	}
	std::cout<<buffer<<std::endl;
	std::string	tmp(buffer);
	if (tmp.find("Connection: keep-alive") != std::string::npos)
		client->setClientConnection(true);
	RequestParse	*request = new RequestParse(buffer);
	client->setClientRequest(request);
	client->setClientPending(client->getClientRequest()->execute_response(client_socket, client));
	if (client->getClientPending() == false)
		std::cout<<YELLOW<<"Closing connection..."<<RESET<<std::endl;
	return (client->getClientPending());
}

bool	continue_connect(int client_socket, Client *client)
{
	client->setClientPending(client->getClientRequest()->execute_response(client_socket, client));
	return (client->getClientPending());
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
	int	server_socket = setup(4243, 10);
	serverskt = server_socket;

	struct epoll_event	event, events[10];
	int	epoll_fd = epoll_create(1);
	int	event_count = 0;
	Client	*clients[10];
	event.events = EPOLLIN;
	event.data.fd = server_socket;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event);
	std::string	buffer[1024];
	while (1)
	{
		signal_decider(0);
		std::cout<<"Waiting..."<<std::endl;
		event_count = epoll_wait(epoll_fd, events, 10, -1);
		for (int i = 0; i < event_count; i++)
		{
			if (events[i].data.fd == server_socket)
			{
				clients[i] = new_connection(server_socket, epoll_fd);
				if (setNonBlocking(clients[i]->getClientSocket()) == -1)
				{
    			    std::cerr << "Failed to set non-blocking mode." << std::endl;
    			    close(clients[i]->getClientSocket());
    			    return (0);
    			}
				std::cout<<GREEN<<"Connection successuful"<<RESET<<std::endl;
			}
			else if (clients[i]->getClientPending() == false)
			{
				//clients[i]->getClientResponse()->setBuffer(buffer);
				clients[i]->setClientPending(handle_connect(events[i].data.fd, clients[i]));
				if (clients[i]->getClientPending() == false)
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
					if (clients[i]->getClientConnection() == false)
						delete clients[i];
				}
			}
			else
			{
				clients[i]->setClientPending(continue_connect(events[i].data.fd, clients[i]));
				if (clients[i]->getClientPending() == false)
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
					if (clients[i]->getClientConnection() == false)
						delete clients[i];
				}
			}
		}
	}
	return (0);
}
