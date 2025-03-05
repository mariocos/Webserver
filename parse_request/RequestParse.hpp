#ifndef	RequestParse_HPP
# define RequestParse_HPP
# include "../webserv.hpp"

class Client;

class RequestParse
{
private:
	/* data */
	std::string method;
	std::string path_to_request;
	std::string HTTP_version;
	std::string Host;
	std::string User;
	std::string Accepts;

public:
	RequestParse();
	RequestParse(const char *request);
	~RequestParse();
	/*  getters  */
	std::string	get_method();
	std::string	get_host();
	std::string	get_httpversion();
	std::string	get_user();
	std::string	get_accetps();
	std::string	get_path();
	/*  setters  */
	void	set_path(std::string path);
	bool	execute_response(int client_socket, Client *client);
	bool	GET_response(int client_socket, Client *client);
	bool	POST_response(int client_socket, Client *client);
	bool	DELETE_response(int client_socket, Client *client);
};

#endif