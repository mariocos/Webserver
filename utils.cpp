#include "includes/webserv.hpp"

std::string	getTimeStamp()
{
	time_t	now = time(NULL);
	struct tm *timeStruct = localtime(&now);
	char	buffer[20];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeStruct);
	return (std::string(buffer));
}

std::string	convertIpToString(struct in_addr s_addr)
{
	uint32_t	addr = ntohl(s_addr.s_addr);
	std::ostringstream	ostream;
	std::string	output;

	for (int i = 3; i >= 0; i--)
	{
		ostream << ((addr >> (i * 8)) & 0xFF);
		if (i > 0)
			ostream << '.';
	}
	output = ostream.str();
	return (output);
}

bool	checkInt(const std::string &str, size_t len)
{
	size_t	n = 0;
	bool	signal = false;
	if (str[n] == '-' || str[n] == '+')
	{
		signal = true;
		n++;
	}
	while (n < len)
	{
		if (!isdigit(str[n]))
			break ;
		n++;
	}
	if (n == len && ((signal == true && len < 11) || (signal == false && len < 10)))
		return (true);
	return (false);
}
/*
	0 -> Bool
	1 -> Int
	2 -> YamlNode
	3 -> std::string
 */
int		returnVariableType(const std::string &value)
{
	if (value == "true" || value == "false" || value == "on" || value == "off" || value == "yes" || value == "no")
		return (0);
	else if (checkInt(value, value.length()))
		return (1);
	else if ("")
		return (2);
	else
		return (3);
}

void	ft_bzero(void *s, size_t n)
{
	char	*p;

	p = reinterpret_cast<char*>(s);
	while (n != 0)
	{
		*p = 0;
		p++;
		n--;
	}
}

int	getMethodRequestedAsInt(std::string method)
{
	if (method == "GET")
		return (GET);
	else if (method == "POST")
		return (POST);
	else if (method == "DELETE")
		return (DELETE);
	else
		return (-1);
}
