#include "../../includes/webserv.hpp"

void	cleaner(Server &server, bool print)
{
	std::vector<Client*>::iterator	clientIt = server.getClientListVector().begin();
	while (clientIt != server.getClientListVector().end())
	{
		if ((*clientIt)->getSocketFd() != -1)
			close((*clientIt)->getSocketFd());
		if ((*clientIt)->getClientOpenFd() != -1)
			close((*clientIt)->getClientOpenFd());
		if (print)
			printLog("INFO", NULL, *clientIt, NULL, 4, "");
		delete (*clientIt);
		server.removeClientFromClientVector(clientIt);
		clientIt = server.getClientListVector().begin();
	}
	std::vector<int>::iterator	errorIt = server.getErrorFdsVector().begin();
	while (errorIt != server.getErrorFdsVector().end())
	{
		if ((*errorIt) > 0)
			close((*errorIt));
		server.removeErrorFdFromErrorVector(errorIt);
		errorIt = server.getErrorFdsVector().begin();
	}
}

void	cleanerForServerCreation(Server &server, bool print)
{
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
			printLog("INFO", *serverIt, NULL, NULL, 2, "");
		delete (*serverIt);
		serverIt++;
	}
}
