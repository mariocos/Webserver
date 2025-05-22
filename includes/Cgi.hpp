#ifndef CGI_HPP
# define CGI_HPP

# include "webserv.hpp"

class Cgi
{
private:
	std::vector<char*>	_envp;
	pid_t	_pid;
	int		_cgiStdIn[2];
	int		_cgiStdOut[2];
	std::vector<uint8_t>	_cgiResponse;
	bool	_running;
public:
	Cgi();
	~Cgi();
	std::vector<char*>	&getEnv();
	std::vector<uint8_t>	&getCgiResponse();
	pid_t	getPid();
	int	*getStdIn();
	int	*getStdOut();
	bool	isActive();
	void	setEnv(std::vector<char*> env);
	void	setPid(pid_t pid);
	void	setCgiResponse(unsigned int buffer_size);
	void	changeCgiState();
};

void	prepareCgi(Client *client);
void	cgiHandler(Server &server, Client *client);

#endif