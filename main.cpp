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

void	new_connection(int server_socket, int epoll_fd)
{
	struct sockaddr_in clientaddr;
	struct epoll_event	event;
	socklen_t	addrlen = sizeof(clientaddr);
	int		client_socket;
	client_socket = accept(server_socket, (sockaddr*)&clientaddr, &addrlen);
	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = client_socket;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event);
	std::cout<<"Socket Number: "<<client_socket<<std::endl;
}

void	*handle_connect(int client_socket)
{
	int	msgsize = 0;
	char	buffer[4096];
	size_t	bytes_read;

	while ((bytes_read = read(client_socket, buffer + msgsize, sizeof(buffer) - msgsize - 1)) > 0)
	{
		msgsize += bytes_read;
		if (msgsize > 4095 || buffer[msgsize - 1] == '\n')
			break ;
	}
	check(bytes_read);
	buffer[msgsize - 1] = '\0';
	std::cout<<buffer<<std::endl;
	RequestParse	request(buffer);
	request.execute_response(client_socket);
	std::cout<<YELLOW<<"Closing connection..."<<RESET<<std::endl;
	return (NULL);
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
	int	server_socket = setup(4243, 200);
	serverskt = server_socket;

	struct epoll_event	event, events[10];
	int	epoll_fd = epoll_create(1);
	int	event_count = 0;
	event.events = EPOLLIN;
	event.data.fd = server_socket;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event);
	while (1)
	{
		signal_decider(0);
		std::cout<<"Waiting..."<<std::endl;
		event_count = epoll_wait(epoll_fd, events, 10, -1);
		for (int i = 0; i < event_count; i++)
		{
			if (events[i].data.fd == server_socket)
			{
				new_connection(server_socket, epoll_fd);
				std::cout<<GREEN<<"Connection successuful"<<RESET<<std::endl;
			}
			else
			{
				handle_connect(events[i].data.fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
			}
		}
	}
	return (0);
}
