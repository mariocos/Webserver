#include "webserv.hpp"

void	cleaner(Server &server, std::vector<Client*> &clientList)
{
	std::vector<Client*>::iterator	clientIt = clientList.begin();
	while (clientIt != clientList.end())
	{
		close((*clientIt)->getClientSocket());
		close((*clientIt)->getClientOpenFd());
		delete (*clientIt);
		clientIt++;
	}
	close(server.getServerSocket());
}
