#include "webserv.hpp"

int serverskt = 0;

void	ctrl_c(int signal, siginfo_t *info, void *context)
{
	(void) info;
	(void) context;
	if (signal == SIGINT)
	{
		close(serverskt);
		exit(1);
	}
}

void	ignore(struct sigaction *sa, int signal)
{
	struct sigaction	original;
	int					original_flags;

	original_flags = sa->sa_flags;
	sa->sa_handler = SIG_IGN;
	sa->sa_flags |= SA_SIGINFO;
	if (sigemptyset(&sa->sa_mask) != 0)
		return ;
	sigaction(signal, sa, &original);
	sa->sa_flags = original_flags;
}

void	signal_decider(int type)
{
	static struct sigaction	sa;

	if (type == 0)
	{
		sa.sa_sigaction = ctrl_c;
		sa.sa_flags = SA_SIGINFO;
		if (sigemptyset(&sa.sa_mask) != 0)
			return ;
		sigaction(SIGINT, &sa, NULL);
		ignore(&sa, SIGQUIT);
	}
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

int	new_connection(int server_socket)
{
	int	client_socket;
	struct sockaddr_in clientaddr;
	socklen_t	addrlen = sizeof(clientaddr);
	check((client_socket = accept(server_socket, (sockaddr*)&clientaddr, &addrlen)));
	return (client_socket);
}

void	*handle_connect(int client_socket)
{
	char	buffer[4096];
	size_t	bytes_read;
	int	msgsize = 0;

	while ((bytes_read = read(client_socket, buffer + msgsize, sizeof(buffer) - msgsize - 1)) > 0)
	{
		msgsize += bytes_read;
		if (msgsize > 4095 || buffer[msgsize - 1] == '\n')
			break ;
	}
	check(bytes_read);
	buffer[msgsize - 1] = '\0';
	std::cout<<buffer<<std::endl;
	write(client_socket, "HTTP/1.1 200 OK\n", 16);
	write(client_socket, "Content-Type: text/plain\n", 25);
	write(client_socket, "Content-Length: 12\n\n", 20);
	write(client_socket, "Hello World!", 12);
	close(client_socket);
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
	std::string	config;
	if (ac == 1)
		config = "default.config";
	else
		config = av[1];
	int	server_socket = setup(4243, 5);
	serverskt = server_socket;
	while (1)
	{
		signal_decider(0);
		std::cout<<"Waiting..."<<std::endl;
		int	client_socket = new_connection(server_socket);
		std::cout<<GREEN<<"Connection successuful"<<RESET<<std::endl;
		handle_connect(client_socket);
	}
	return (0);
}
