#include "../../../includes/webserv.hpp"

std::vector<Client*>::iterator	getRightHole(Server &server, int event_fd)
{
	std::vector<Client*>::iterator	it = server.getClientListVector().begin();
	std::vector<Client*>::iterator	end = server.getClientListVector().end();
	while (it != end)
	{
		if (*it != NULL && (*it)->getSocketFd() == event_fd)
			return (it);
		it++;
	}
	return (end);
}
