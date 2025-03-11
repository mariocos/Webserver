#include "webserv.hpp"

//10 tem que ser mudado conforme o config file

int	getNewHole(Client **clients)
{
	if (!clients)
		return (-1);
	for (int i = 0; i < 10; i++)
	{
		if (!clients[i])
			return (i);
	}
	return (-1);
}

int	getRightHole(Client **clients, int event_fd)
{
	if (!clients)
		return (-1);
	for (int i = 0; i < 10; i++)
	{
		if (!clients[i])
			continue;
		if (clients[i]->getClientSocket() == event_fd)
			return (i);
	}
	return (-1);
}

int	getPendingHole(Client **clients)
{
	if (!clients)
		return (-1);
	for (int i = 0; i < 10; i++)
	{
		if (!clients[i])
			continue;
		if (clients[i]->getClientWritingFlag() == false)
			return (i);
	}
	return (-1);
}

int	getNextPendingHole(Client **clients, int i)
{
	if (!clients || i == 10)
		return (-1);
	i++;
	while (i < 10)
	{
		if (!clients[i])
		{
			i++;
			continue;
		}
		if (clients[i]->getClientWritingFlag() == false)
			return (i);
		i++;
	}
	return (-1);
}
