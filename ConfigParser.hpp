#ifndef	CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "webserv.hpp"

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

class ConfigParser
{
private:
	int	_port;
	int	_maxBodySize;
	std::string	_host;
	std::string	_name;
	std::string	_error;
	std::string	_root;
	std::string	_index;
public:
	ConfigParser();
	ConfigParser(const char *file);
	ConfigParser(const ConfigParser &copy);
	ConfigParser& operator=(const ConfigParser &copy);
	~ConfigParser();
};

#endif
