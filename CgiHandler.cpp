#include "includes/webserv.hpp"

void	prepareCgi(Client *client)
{
	std::vector<std::string>	env;
	std::ostringstream	number;
	std::string	lenght;

	env.push_back("REQUEST_METHOD="+ client->getClientRequest()->get_method());
	env.push_back("SCRIPT_NAME="+ client->getClientRequest()->get_path());
	//need to implement a member function to get whats behind the "?" after the file path
	//env.push_back("QUERY_STRING=" + client->getClientRequest()->get_name());
	number << client->getClientFile()->getFileStats()->st_size;
	lenght = number.str();
	env.push_back("CONTENT_LENGHT=" + lenght);
	env.push_back("CONTENT_TYPE=" + client->getClientRequest()->get_content_type());
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=WebServ/1.0");
	env.push_back("SERVER_NAME=" + client->getDomainTriggered());
	number.clear();
	number << client->getPortTriggered();
	lenght = number.str();
	env.push_back("SERVER_PORT=" + lenght);
	//need to implement a member function to get the client address and port
	//env.push_back("REMOTE_ADDR=" + client->getClientAddr());
	//env.push_back("REMOTE_PORT=" + client->getClientPort());
	std::vector<char*>	envp;
	for (size_t i = 0; i < env.size(); i++)
	{
		envp.push_back(const_cast<char*>(env[0].c_str()));
	}
	envp.push_back(NULL);
	client->setClientCgi(new Cgi());
	client->getClientCgi()->setEnv(envp);
	pipe(client->getClientCgi()->getStdIn());
	pipe(client->getClientCgi()->getStdOut());
}

void	cgiHandler(Server &server, Client *client)
{
	if (client->getClientReadingFlag() && !client->getClientWritingFlag() && !client->getClientCgi()->isActive())
	{
		prepareCgi(client);
		client->getClientCgi()->setPid(fork());
		if (client->getClientCgi()->getPid() == 0)
		{
			std::string	path = client->getClientRequest()->get_path();
			dup2(client->getClientCgi()->getStdIn()[0], STDIN_FILENO);
			dup2(client->getClientCgi()->getStdOut()[1], STDOUT_FILENO);
			close(client->getClientCgi()->getStdIn()[1]);
			close(client->getClientCgi()->getStdOut()[0]);
			char	*av[] = {(char*)"/usr/bin/python3", const_cast<char*>(path.c_str()), NULL};
			execve("/usr/bin/python3", av, client->getClientCgi()->getEnv().data());
			exit(1);
		}
		struct epoll_event	event;
		event.events = EPOLLIN | EPOLLRDHUP;
		event.data.fd = client->getClientCgi()->getStdOut()[0];
		epoll_ctl(server.getEpollFd(), EPOLL_CTL_ADD, event.data.fd, &event);
		close(client->getClientCgi()->getStdIn()[0]);
		close(client->getClientCgi()->getStdOut()[1]);
		client->setClientReadingFlag(true);
		client->setClientWritingFlag(false);
		close(client->getClientCgi()->getStdIn()[1]);
		client->getClientCgi()->changeCgiState();
	}
	else if (client->getClientReadingFlag())
	{
		client->getClientCgi()->setCgiResponse(1048576);
		close(client->getClientCgi()->getStdOut()[0]);
		client->getClientCgi()->changeCgiState();
		client->setClientReadingFlag(false);
		client->setClientWritingFlag(true);
	}
	else if (client->getClientWritingFlag())
	{
		client->getClientResponse()->clearResponse();
		client->getClientResponse()->addToResponse(client->getClientRequest()->get_httpversion() + " 200 ok\r\n");
		client->getClientResponse()->addToResponse("Content-Type: " + client->getClientResponse()->getType() + "\r\n");
		client->getClientResponse()->addToResponseLenght(client->getClientCgi()->getCgiResponse().size());
		client->getClientResponse()->addToResponse("Connection: close\r\n");
		client->getClientResponse()->addToResponse("Content-Length: " + client->getClientResponse()->getResponseLenghtAsString() + "\r\n\r\n");
		sendMsgToSocket(client->getSocketFd(), client->getClientResponse()->getResponse().size(), client, client->getClientResponse());
		client->getClientResponse()->clearResponse();
		std::vector<uint8_t> data = client->getClientCgi()->getCgiResponse();
		if (send(client->getSocketFd(), reinterpret_cast<const char*>(data.data()), client->getClientCgi()->getCgiResponse().size(), MSG_NOSIGNAL) == -1)
			throw SendException(client, client->getClientResponse());
		client->setClientReadingFlag(true);
		client->setClientWritingFlag(false);
	}
}
