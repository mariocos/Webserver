#include "includes/webserv.hpp"

bool	isConnectionGood(Server &server, std::vector<Client*>::iterator it)
{
	std::vector<ServerBlock*>	copy = server.getServerBlocks();
	std::vector<ServerBlock*>::iterator	serverIt = copy.begin();
	while (serverIt != copy.end())
	{
		if ((*it)->getPortTriggered() == (*serverIt)->getBlockPort())
		{
			if ((*it)->getClientRequest()->get_host() == (*serverIt)->getBlockName())
				return (true);
		}
		serverIt++;
	}
	return (false);
}

bool	doesPortsMatch(Server &server, std::vector<Client*>::iterator it)
{
	std::vector<ServerBlock*>	copy = server.getServerBlocks();
	std::vector<ServerBlock*>::iterator	serverIt = copy.begin();
	while (serverIt != copy.end())
	{
		if ((*it)->getPortTriggered() == (*serverIt)->getBlockPort())
			return (true);
		serverIt++;
	}
	return (false);
}

void	handlePortOrDomainMismatch(Server &server, std::vector<Client*>::iterator it)
{
	if (doesPortsMatch(server, it))
		throw RedirectException(server, it);
	(*it)->setClientOpenFd(-1);
	(*it)->removeSocketFromEpoll((*it)->getSocketFd());
	(*it)->getServerBlockTriggered()->decreaseConnections();
	delete (*it);
	server.removeClientFromClientVector(it);
}

void	searchForTimeOut(Server &server)
{
	std::vector<Client*>::iterator	it = server.getClientListVector().begin();
	if (it == server.getClientListVector().end())
		return ;
	while (it != server.getClientListVector().end())
	{
		if ((*it)->hasTimedOut())
		{
			loadError408((*it)->getSocketFd(), (*it)->getClientResponse(), (*it));
			printLog("INFO", NULL, (*it), (*it)->getClientResponse(), 12, "");
			(*it)->removeSocketFromEpoll((*it)->getSocketFd());
			close((*it)->getSocketFd());
			(*it)->getServerBlockTriggered()->decreaseConnections();
			delete (*it);
			server.removeClientFromClientVector(it);
			it = server.getClientListVector().begin();
			if (it == server.getClientListVector().end())
				return ;
		}
		it++;
	}
}
