#include "includes/webserv.hpp"

void	cleaner(Server &server, std::vector<Client*> &clientList, bool print)
{
	std::vector<Client*>::iterator	clientIt = clientList.begin();
	while (clientIt != clientList.end())
	{
		if ((*clientIt)->getSocketFd() != -1)
			close((*clientIt)->getSocketFd());
		if ((*clientIt)->getClientOpenFd() != -1)
			close((*clientIt)->getClientOpenFd());
		delete (*clientIt);
		clientIt++;
	}
	std::vector<ServerBlock*>	copy = server.getServerBlocks();
	std::vector<ServerBlock*>::iterator	serverIt = copy.begin();
	while (serverIt != copy.end())
	{
		std::vector<Routes*>::iterator	routeIt = (*serverIt)->getRoutesVector().begin();
		while (routeIt != (*serverIt)->getRoutesVector().end())
		{
			delete (*routeIt);
			routeIt++;
		}
		if (print)
			printLog("INFO", *serverIt, NULL, NULL, 2);
		delete (*serverIt);
		serverIt++;
	}
}
