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

int	find_error_hole(int *error_fd)
{
	for (int i = 0; i < 10; i++)
	{
		if (error_fd[i] == -1)
			return (i);
	}
	return (-1);
}

void	error_handler(int error_socket)
{
	std::string response;
	response.append("HTTP/1.1 503 Service Unavailable\r\n");
	response.append("Date: Wed, 06 Mar 2024 12:00:00 GMT\r\n");
	response.append("Server: WebServ\r\n");
	response.append("Content-Type: text\r\n");
	response.append("Content-Lenght: 0\r\n");
	response.append("Retry-After: 10\r\n\r\n");
	send(error_socket, response.c_str(), response.length(), O_NONBLOCK);
	close(error_socket);
}

int	error_connection(int server_socket, int epoll_fd)
{
	struct sockaddr_in clientaddr;
	struct epoll_event	event;
	socklen_t	addrlen = sizeof(clientaddr);
	int	client_socket = accept(server_socket, (sockaddr*)&clientaddr, &addrlen);
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = client_socket;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event);
	return (client_socket);
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

	struct epoll_event	event, events[11];
	int	epoll_fd = epoll_create(1);
	int	event_count = 0;
	Client	*clients[10] = {};
	int	error_socket[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	event.events = EPOLLIN;
	event.data.fd = server_socket;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event);
	while (1)
	{
		signal_decider(0);
		std::cout<<"Waiting..."<<std::endl;
		event_count = epoll_wait(epoll_fd, events, 10, 60);
		if (event_count == 0)
		{
			int	n = getPendingHole(clients);
			if (n == -1)
			{
				event_count = epoll_wait(epoll_fd, events, 10, -1);
				std::cout<<RED<<"Nothing Pending"<<RESET<<std::endl;
				continue;
			}
			while (n != -1)
			{
				clients[n]->handle_connect(clients[n]->getClientSocket());
				if (clients[n]->getClientWritingFlag() == true)
				{
					int	eventnb = findEventFd(clients[n], events);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[eventnb].data.fd, &events[eventnb]);
					delete clients[n];
					clients[n] = NULL;
				}
				n = getNextPendingHole(clients, n);
			}
		}
		else
		{
			for (int i = 0; i < event_count; i++)
			{
				for (int z = 0; z < 10; z++)
				{
					if (events[i].data.fd == error_socket[z])
					{
						error_handler(events[i].data.fd);
						break;
					}
				}
				if (events[i].data.fd == server_socket)
				{
					int	n = getNewHole(clients);
					if (n == -1)
					{
						int	e = find_error_hole(error_socket);
						error_socket[e] = error_connection(server_socket, epoll_fd);
						std::cout<<RED<<"All spaces ocupied"<<RESET<<std::endl;
						continue;
					}
					clients[n] = new_connection(server_socket, epoll_fd);
					if (setNonBlocking(clients[n]->getClientSocket()) == -1)
					{
    				    std::cerr << "Failed to set non-blocking mode." << std::endl;
    				    close(clients[n]->getClientSocket());
    				    return (0);
    				}
					std::cout<<GREEN<<"Connection successuful"<<RESET<<std::endl;
				}
				else
				{
					int	n = getRightHole(clients, events[i].data.fd);
					if (n == -1)
					{
						std::cout<<RED<<"All spaces ocupied"<<RESET<<std::endl;
						continue;
					}
					else if (clients[n]->getClientReadingFlag() == false)
					{
						clients[n]->readRequest(clients[n]->getClientSocket());
						clients[n]->setClientWritingFlag(false);
					}
					else
					{
						clients[n]->handle_connect(events[i].data.fd);
						if (clients[n]->getClientWritingFlag() == true)
						{
							epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
							delete clients[n];
							clients[n] = NULL;
						}
					}
				}
			}
		}
	}
	return (0);
}
