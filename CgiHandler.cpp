#include "includes/webserv.hpp"

void	prepareCgi(Client *client)
{
	std::vector<std::string>	env;
	std::string	path;
	std::string	name;

	env.push_back("REQUEST_METHOD="+ client->getClientRequest()->get_method());
	name = client->getClientRequest()->get_path().substr(client->getClientRequest()->get_path().rfind("/") + 1);
	env.push_back("SCRIPT_NAME="+ name);
	path = "website" + client->getClientRequest()->get_path();
	env.push_back("QUERY_STRING=" + client->getClientRequest()->get_query_str());
	client->getClientFile()->openFile(path.c_str(), client->getSocketFd());
	env.push_back("CONTENT_LENGHT=" + transformToString(client->getClientFile()->getFileStats()->st_size));
	client->getClientFile()->getFile()->close();
	env.push_back("CONTENT_TYPE=" + client->getClientRequest()->get_content_type());
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=WebServ/1.0");
	env.push_back("SERVER_NAME=" + client->getDomainTriggered());
	env.push_back("SERVER_PORT=" + transformToString(client->getPortTriggered()));
	env.push_back("REMOTE_ADDR=" + client->getClientIp());
	env.push_back("REMOTE_PORT=" + transformToString(client->getClientPort()));
	client->setClientCgi(new Cgi());
	client->getClientCgi()->setEnv(env);
	pipe(client->getClientCgi()->getStdIn());
	pipe(client->getClientCgi()->getStdOut());
}

void	cgiHandler(Server &server, Client *client)
{
	try
	{
		if (client->getClientReadingFlag() && !client->getClientWritingFlag() && !client->getClientCgi())
		{
			prepareCgi(client);
			printLog("CGI", client->getServerBlockTriggered(), client, client->getClientResponse(), 7);
			client->getClientCgi()->setPid(fork());
			if (client->getClientCgi()->getPid() == 0)
				client->getClientCgi()->executeCgi(client);
			else
			{
				struct epoll_event	event;
				event.events = EPOLLIN | EPOLLRDHUP;
				event.data.fd = client->getClientCgi()->getStdOut()[0];
				epoll_ctl(server.getEpollFd(), EPOLL_CTL_ADD, event.data.fd, &event);
				close(client->getClientCgi()->getStdIn()[0]);
				close(client->getClientCgi()->getStdOut()[1]);
				client->setClientReadingFlag(true);
				client->setClientWritingFlag(false);
				//if CGI is set to Post need to write to STDIN[1] so cgi can read it
				//if (client->getClientRequest()->get_method() == "POST")
				//	send(client->getClientCgi()->getStdIn()[1], client->getClientRequest()->get_buffer().c_str(), lenght, MSG_NOSIGNAL);
				close(client->getClientCgi()->getStdIn()[1]);
				client->getClientCgi()->changeCgiState();
				client->getClientResponse()->setStatusCode(200);
			}
		}
		else if (client->getClientReadingFlag())
			client->getClientCgi()->readCgiResponse(server, client);
		else if (client->getClientWritingFlag())
			client->getClientCgi()->writeCgiResponse(client);
	}
	catch(const std::exception& e)
	{
		if (std::string(e.what()) == "Bad child")
			throw BadChildException();
		std::cerr << e.what() << '\n';
	}
}

std::vector<Client*>::iterator	isThisPipe(int fd, std::vector<Client*> &clientList)
{
	std::vector<Client*>::iterator	it = clientList.begin();
	std::vector<Client*>::iterator	end = clientList.end();
	while (it != end)
	{
		if ((*it)->getClientCgi() && (*it)->getClientCgi()->getStdOut()[0] == fd)
			return (it);
		it++;
	}
	return (end);
}
