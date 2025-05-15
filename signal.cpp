#include "includes/webserv.hpp"

void	cleaner(Server &server, std::vector<Client*> &clientList)
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
	for (size_t i = 0; i < server.getServerBlocks().size(); i++)
	{
		close(server.getServerBlock(i).getSocketFd());
	}
}
