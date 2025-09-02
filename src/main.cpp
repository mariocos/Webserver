#include "../includes/webserv.hpp"

bool	run;

NoPendingConnectionsException::NoPendingConnectionsException() :
runtime_error("")
{
}

NewConnectionCreationException::NewConnectionCreationException(Server &server) :
runtime_error(RED "Error adding a new client" RESET)
{
	cleaner(server, true);
}

SendException::SendException(Client *client, Response *response) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] Error while sending to " + client->getClientIp() + ":" + transformToString(client->getClientPort()) + RESET)
{
	response->clearResponse();
	client->setClientWritingFlag(true);
	client->setClientPending(false);
	client->getClientFile()->setReading(false);
}

ReadException::ReadException(Client *client, Response *response) :
runtime_error("[" + getTimeStamp() + "]" + RED + " [ERROR] Error while reading from " + client->getClientIp() + ":" + transformToString(client->getClientPort()) + RESET)
{
	response->clearResponse();
	client->setClientReadingFlag(true);
	client->setClientPending(false);
	client->getClientFile()->setWriting(false);
}

RedirectException::RedirectException(Server &server, std::vector<Client*>::iterator it) :
runtime_error("[" + getTimeStamp() + "]" + YELLOW + " [INFO] Redirecting Request from " + (*it)->getClientIp() + ":" + transformToString((*it)->getClientPort()) + " to Default Server Block" + RESET)
{
	std::vector<ServerBlock*>	serverBlocks = server.getServerBlocks();
	std::vector<ServerBlock*>::iterator	serverIt = serverBlocks.begin();
	while (serverIt != serverBlocks.end())
	{
		if ((*serverIt)->isDefault())
			break;
		serverIt++;
	}
	load301((*it)->getSocketFd(), (*it)->getClientResponse(), (*it), "http://" + (*serverIt)->getBlockName() + ":" + transformToString((*serverIt)->getBlockPort()) + (*it)->getClientRequest()->get_path());
}

BadChildException::BadChildException() :
runtime_error("Bad child")
{
}

BadPipeCreationException::BadPipeCreationException() :
runtime_error(RED "Error opening the pipe" RESET) {}

NonBlockingException::NonBlockingException(int fd) :
runtime_error(RED "Failed To Set Server Fd To Non Blocking" RESET)
{
	if (fd != -1)
		close(fd);
}

BadClientException::BadClientException() :
runtime_error("BadClient")
{
}

EmptyBufferException::EmptyBufferException() :
runtime_error("EmptyBuffer")
{
}

NoUploadPathException::NoUploadPathException(Server &server, std::vector<Routes*> tmp) :
runtime_error(RED "No Upload Path setted for a POST route" RESET)
{
	close(server.getEpollFd());
	std::vector<Routes*>::iterator	it = tmp.begin();
	while (it != tmp.end())
	{
		delete (*it);
		it++;
	}
	cleanerForServerCreation(server, true);
}

ConfigFileStructureException::ConfigFileStructureException(std::string key) :
runtime_error(RED"Config file error: " + key + RESET) {}

MessagelessException::MessagelessException(std::string msg) :
runtime_error(msg) {}

void	error_connection_handler(Server &server)
{
	std::vector<int>::iterator	it = server.getErrorFdsVector().begin();
	std::vector<int>::iterator	end = server.getErrorFdsVector().end();
	if (it == end)
		return ;
	for (int i = 0; i < server.getEpollCount(); i++)
	{
		it = server.getErrorFdsVector().begin();
		epoll_event &event = server.getEpollIndex(i);
		while (it != server.getErrorFdsVector().end())
		{
			if (*it == event.data.fd && ((event.events & EPOLLIN) || (event.events & EPOLLOUT)))
			{
				loadError503((*it));
				server.removeFromEpoll((*it));
				server.removeErrorFdFromErrorVector(it);
				break;
			}
			it++;
		}	
	}
}

void	stopRunning(int signal)
{
	(void)signal;
	run = false;
	std::cout<<std::endl;
}

int	main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cout<<RED<<"Wrong number of arguments"<<RESET<<std::endl;
		return (1);
	}
	try
	{
//		std::vector<int>	ports;
//		ports.push_back(4243);
//		ports.push_back(8080);
//		ports.push_back(3000);
//		ports.push_back(2424);
//		std::vector<std::string>	names;
//		names.push_back("localhost");
//		names.push_back("webserver.com");
//		names.push_back("127.0.0.1");
//		names.push_back("script");
//		Server	server(ports, names, 20);
        YamlParser parser;
        YamlNode* root = NULL;
//           std::vector<ServerBlock*>	Servers = NULL;
		std::string filePath;
		if (av[1])
			filePath = av[1];
		else
			filePath = "config_files/simple.yaml";
		std::string fileName;
		if (filePath.find('/') != std::string::npos)
			fileName = filePath.substr(filePath.rfind('/') + 1);
		else
			fileName = filePath;
        root = parser.parse(filePath);
        if (!root) {
            std::cerr << RED"Failed to parse YAML file."RESET << std::endl;
			return (1);
        }
		Server server = Server(root);
		delete root;
		if (fileName.empty())
			printLog("INFO", NULL, NULL, NULL, 1, "default.yaml");
		else
			printLog("INFO", NULL, NULL, NULL, 1, fileName);
		run = true;
		signal(SIGINT, stopRunning);
		while (run)
		{
			server.setEpollCount(epoll_wait(server.getEpollFd(), server.getEpollEventArray(), server.getMaxEvents(), 100));
			if (server.getEpollCount() == -1)
			{
				cleaner(server, true);
				return (1);
			}
			try
			{
				server.handle_connections();
				searchForTimeOut(server);
			}
			catch(const std::exception& e)
			{
				//in case of a lost child proccess after a error in execve
				if (std::string(e.what()) == "Bad child")
				{
					cleaner(server, false);
					throw BadChildException();
				}
				std::cerr << e.what() << '\n';
			}
		}
		cleaner(server, true);
	}
	catch(const std::exception& e)
	{
		if (std::string(e.what()) == "Bad child")
			return (0);
		std::cerr << e.what() << '\n';
	}
	return (0);
}


/*

	Lista de Erros:
		- Settings vazio da conditional jump
		- Interpreters vazio da segfault
		- Se tentar dar set a outro serverBlock como default ele da caca (acho que esta mas vou testar mais)

*/

//command to test GET to another serverBlock besides the localhost : "curl -H "Host: webserver.com" http://localhost:8080"
//command to test POST : "curl -X POST -d "whatever we want to send as content" -H "Host: webserver.com" http://localhost:8080/chicken-jokey"
//command to test POST with binary : "curl -X POST --data-binary @/bin/ls -H "Host: webserver.com" http://localhost:8080/chicken-jokey"
//command to test GET to CGI : "curl -H "Host: script" http://localhost:2424/cgi-bin/hello.py\?name\=Jonh\&age\=30"
//command to test POST to CGI : "curl -X POST -H "Host: script" -d "name=paul&lang=python" http://localhost:2424/cgi-bin/hello.py\?name\=Jonh\&age\=30"
//command to test 1000 requests with siege : "siege -b -c 50 -r 20 http://localhost:4243/index.html"
//command to test 1000 requests with siege : "siege -b -c 100 -r 10 http://localhost:4243/index.html"
//command to test 1000 requests with siege : "siege -b -c 200 -r 5 http://localhost:4243/index.html"
