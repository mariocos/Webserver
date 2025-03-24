#include "webserv.hpp"

//10 tem que ser mudado conforme o config file
std::vector<Client*>::iterator	getRightHole(std::vector<Client*> &clientList, int event_fd)
{
	std::vector<Client*>::iterator	it = clientList.begin();
	std::vector<Client*>::iterator	end = clientList.end();
	while (it != end)
	{
		if (*it != NULL && (*it)->getClientSocket() == event_fd)
			return (it);
		it++;
	}
	return (end);
}

std::vector<Client*>::iterator	getPendingHole(std::vector<Client*> &clientList)
{
	std::vector<Client*>::iterator	it = clientList.begin();
	std::vector<Client*>::iterator	end = clientList.end();
	while (it != end)
	{
		if (*it != NULL && ((*it)->getClientWritingFlag() == false))
			return (it);
		it++;
	}
	return (end);
}

std::vector<Client*>::iterator	getNextPendingHole(std::vector<Client*> &clientList, std::vector<Client*>::iterator it)
{
	std::vector<Client*>::iterator	end = clientList.end();
	if (it != end)
		it++;
	while (it != end)
	{
		if (*it != NULL && (*it)->getClientWritingFlag() == false)
			return (it);
		it++;
	}
	return (end);
}
