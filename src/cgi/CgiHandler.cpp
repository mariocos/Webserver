#include "../../includes/webserv.hpp"

void	prepareCgi(Client *client)
{
	std::vector<std::string>	env;
	std::string	path;
	std::string	name;

	env.push_back("REQUEST_METHOD="+ client->getClientRequest()->get_method());
	name = client->getClientRequest()->get_path().substr(client->getClientRequest()->get_path().rfind("/") + 1);
	env.push_back("SCRIPT_NAME="+ name);
	path = client->getNewPath();
	env.push_back("QUERY_STRING=" + client->getClientRequest()->get_query_str());
	if (client->getClientRequest()->get_method() == "POST")
		env.push_back("CONTENT_LENGHT=" + transformToString(client->getClientRequest()->get_content().size()));
	else
	{
		client->getClientFile()->openFile(path.c_str(), client->getSocketFd());
		env.push_back("CONTENT_LENGHT=" + transformToString(client->getClientFile()->getFileStats()->st_size));
		client->getClientFile()->getFile()->close();
	}
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
	if (pipe(client->getClientCgi()->getStdIn()) == -1)
		throw BadPipeCreationException();
	if (pipe(client->getClientCgi()->getStdOut()) == -1)
		throw BadPipeCreationException();
}

void	cgiHandler(Server &server, Client *client, int fdTriggered)
{
	try
	{
		if (client->getClientResponse()->getBytesToSend() != 0 && \
			client->getClientResponse()->getBytesToSend() == client->getClientResponse()->getBytesSent())
			return ;
		if (client->getClientReadingFlag() && !client->getClientWritingFlag() && !client->getClientCgi())
		{
			int	methodAsInt = getMethodRequestedAsInt(client->getClientRequest()->get_method());
			if (!client->getRouteTriggered()->canDoMethod(methodAsInt))
				throw Error405Exception(client->getSocketFd(), client->getClientResponse(), client);
			std::string	fileExtension = findFileExtension(client->getClientRequest()->get_path());
			if (!client->getRouteTriggered()->getCgiFileExtension().empty() && fileExtension != "." + client->getRouteTriggered()->getCgiFileExtension())
				throw Error404Exception(client->getSocketFd(), client->getClientResponse(), client);
			prepareCgi(client);
			printLog("CGI", NULL, client, client->getClientResponse(), 7, "");
			client->getClientCgi()->setPid(fork());
			if (client->getClientCgi()->getPid() == 0)
			client->getClientCgi()->executeCgi(client);
			else
			client->getClientCgi()->parentWork(server, client);
		}
		else if (client->getClientCgi() && client->getClientCgi()->getStdIn()[1] != -1 && fdTriggered == client->getClientCgi()->getStdIn()[1])
			client->getClientCgi()->writeBody(server, client);
		else if (client->getClientFile()->isReading())
			client->getClientCgi()->readCgiResponse(server, client);
		else if (client->getClientFile()->isWriting())
			client->getClientCgi()->writeCgiResponse(client);
		client->resetTimer();
	}
	catch(const std::exception& e)
	{
		if (std::string(e.what()) == "Bad child")
			throw BadChildException();
		std::cerr << e.what() << '\n';
		throw BadClientException();
	}
}

std::vector<Client*>::iterator	isThisPipe(int fd, Server &server)
{
	std::vector<Client*>::iterator	it = server.getClientListVector().begin();
	std::vector<Client*>::iterator	end = server.getClientListVector().end();
	while (it != end)
	{
		if ((*it)->getClientCgi() && ((*it)->getClientCgi()->getStdOut()[0] == fd || (*it)->getClientCgi()->getStdIn()[1] == fd))
			return (it);
		it++;
	}
	return (end);
}
