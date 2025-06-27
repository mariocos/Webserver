#ifndef	ROUTES_HPP
# define ROUTES_HPP

# define RESET "\033[0m"
# define GREEN "\033[1m\033[32m"
# define RED "\033[1m\033[31m"
# define YELLOW "\033[1m\033[33m"

# include "webserv.hpp"

class Routes
{
    private:
        std::string _root;
		std::string	_uri;
		std::string	_redirectPath;
		std::string	_savedPath;
        int			_maxConnections;
		int			_maxBodySize;
        bool		_methods[3];
		bool		_defaultRoute;
        bool		_isCgi;
		bool		_directoryListing;
		bool		_isPermanentRedirect;
		bool		_isTemporaryRedirect;
    public:
        Routes();
		Routes(int maxConnections, int maxBodySize, bool flag, std::string root, std::string uri);
		Routes(const Routes &copy);
		Routes	&operator=(const Routes &copy);
        ~Routes();
		std::string	&getRoot();
		std::string	&getURI();
		std::string	&getRedirectPath();
		std::string	&getSavedPath();
		int			getMaxConnections();
		int			getMaxBodySize();
		bool		canDoMethod(int method);
		bool		isDefault();
		bool		isCgi();
		bool		isListing();
		bool		isPermanentRedirect();
		bool		isTemporaryRedirect();
		void		setRoot(std::string &root);
		void		setURI(std::string &uri);
		void		setRedirectPath(std::string &path);
		void		setSavedPath(std::string &path);
		void		setMaxConnections(int value);
		void		setMaxBodySize(int value);
		void		setMethod(int method, bool flag);
		void		setAsCgi();
		void		setAsListing();
		void		setAsPermanentRedirect();
		void		setAsTemporaryRedirect();
};

std::string	generateListingHTML(std::string &dirPath, Client *client);
std::string	constructListingPath(Client *client);

class	LoadListingException : public std::runtime_error
{
	public:
		LoadListingException(int client_socket, Response *response, Client *client);
};

#endif