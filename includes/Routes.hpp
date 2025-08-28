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
		std::string	_cgiFileExtension;
		std::string	_defaultFileForDirectory;
		std::string	_uploadPath;
		int			_maxBodySize;
        bool		_methods[3];
		bool		_defaultRoute;
        bool		_isCgi;
		bool		_directoryListing;
		bool		_isPermanentRedirect;
		bool		_isTemporaryRedirect;
		bool		_yamlHasDefault;
    public:
        Routes();
		Routes(int maxBodySize, bool flag, std::string root, std::string uri);
		Routes(const Routes &copy);
		Routes	&operator=(const Routes &copy);
        ~Routes();
		std::string	&getRoot();
		std::string	&getURI();
		std::string	&getRedirectPath();
		std::string	&getSavedPath();
		std::string	&getCgiFileExtension();
		std::string	&getDefaultFile();
		std::string	getDefaultPathForDirectoryRequest();
		std::string	&getUploadPath();
		int			getMaxBodySize();
		bool		canDoMethod(int method);
		bool		isDefault();
		bool		isCgi();
		bool		isListing();
		bool		isPermanentRedirect();
		bool		isTemporaryRedirect();
		bool		doesYamlHasDefault();
		void		setRoot(std::string &root);
		void		setURI(std::string &uri);
		void		setRedirectPath(std::string &path);
		void		setSavedPath(std::string &path);
		void		setCgiFileExtension(std::string &extension);
		void		setDefaultFileForDirectory(std::string &file_name);
		void		setUploadPath(std::string &path);
		void		setMaxBodySize(int value);
		void		setMethod(int method, bool flag);
		void		setAsCgi();
		void		setAsListing();
		void		setAsPermanentRedirect();
		void		setAsTemporaryRedirect();
		void		setAsDefaultRoute();
		void		yamlDoesHaveDefault();
};

std::string	generateListingHTML(std::string &dirPath, Client *client);
std::string	constructListingPath(Client *client);

class	LoadListingException : public std::runtime_error
{
	public:
		LoadListingException(int client_socket, Response *response, Client *client);
};

#endif