#ifndef	ROUTES_HPP
# define ROUTES_HPP

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

# include "webserv.hpp"

enum methods {
	GET,
	POST,
	DELETE,
};

class Routes
{
    private:
        std::string _root;
        int			_maxConnections;
        bool		_methods[3];
        bool		_isCgi;
    public:
        Routes();
        ~Routes();
};

#endif