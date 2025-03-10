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

void	make_response(int client_socket, char *buffer)
{
	std::string	response, r_buffer, method, requested_file, http_version, path, type = "text/plain";
	std::ifstream	input;
	std::istringstream	request(buffer);

	request >> method >> requested_file >> http_version;
	if (requested_file == "/")
		requested_file = "/index.html";
	if (requested_file.length() > 5 && requested_file.find(".html") == requested_file.length() - 5)
		type = "text/html";
	else if (requested_file.length() > 4 && requested_file.find(".css") == requested_file.length() - 4)
		type = "text/css";
	else
		type = "text/html";
	response.append("HTTP/1.1 200 OK\n");
	response.append("Content-Type: " + type + "\n\n");
	path = "website" + requested_file;
	input.open(path.c_str());
	if (input.is_open())
	{
		while (getline(input, r_buffer))
		{
			response.append(r_buffer);
			response.append("\n");
		}
	}
	else
	{
		input.open("website/404.html");
		while (getline(input, r_buffer))
		{
			response.append(r_buffer);
			response.append("\n");
		}
	}
	input.close();
	write(client_socket, response.c_str(), response.length());
	close(client_socket);
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
	make_response(client_socket, buffer);
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
	std::string	config = "default.config";
	if (ac == 2)
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
