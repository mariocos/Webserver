#include "webserv.hpp"

int serverskt;

void	ctrl_c(int signal, siginfo_t *info, void *context)
{
	(void) info;
	(void) context;
	if (signal == SIGINT)
	{
		close(serverskt);
		exit(1);
	}
}

void	ignore(struct sigaction *sa, int signal)
{
	struct sigaction	original;
	int					original_flags;

	original_flags = sa->sa_flags;
	sa->sa_handler = SIG_IGN;
	sa->sa_flags |= SA_SIGINFO;
	if (sigemptyset(&sa->sa_mask) != 0)
		return ;
	sigaction(signal, sa, &original);
	sa->sa_flags = original_flags;
}

void	signal_decider(int type)
{
	static struct sigaction	sa;

	if (type == 0)
	{
		sa.sa_sigaction = ctrl_c;
		sa.sa_flags = SA_SIGINFO;
		if (sigemptyset(&sa.sa_mask) != 0)
			return ;
		sigaction(SIGINT, &sa, NULL);
		ignore(&sa, SIGQUIT);
	}
}
