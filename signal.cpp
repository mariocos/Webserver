#include "includes/webserv.hpp"

void	cleaner(Server &server, std::vector<Client*> &clientList)
{
	std::vector<Client*>::iterator	clientIt = clientList.begin();
	std::vector<ServerBlock*>	copy = server.getServerBlocks();
	std::vector<ServerBlock*>::iterator	serverIt = copy.begin();
	while (clientIt != clientList.end())
	{
		if ((*clientIt)->getSocketFd() != -1)
			close((*clientIt)->getSocketFd());
		if ((*clientIt)->getClientOpenFd() != -1)
			close((*clientIt)->getClientOpenFd());
		delete (*clientIt);
		clientIt++;
	}
	while (serverIt != copy.end())
	{
		if ((*serverIt)->getSocketFd() != -1)
			close((*serverIt)->getSocketFd());
		delete (*serverIt);
		serverIt++;
	}
}
