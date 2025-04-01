#include "includes/webserv.hpp"

void	cleaner(Server &server, std::vector<Client*> &clientList)
{
	std::vector<Client*>::iterator	clientIt = clientList.begin();
	while (clientIt != clientList.end())
	{
		if ((*clientIt)->getClientSocket() != -1)
			close((*clientIt)->getClientSocket());
		if ((*clientIt)->getClientOpenFd() != -1)
			close((*clientIt)->getClientOpenFd());
		delete (*clientIt);
		clientIt++;
	}
	close(server.getServerSocket());
}
