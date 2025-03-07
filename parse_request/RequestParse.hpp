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
	std::string	*_buffer;
	std::string	content_type;
	std::string	content_length;
	std::string	connection;
	int			error_code;
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
	std::string	get_content_type();
	std::string	get_content_length();
	void		buildRequest(const char *req);
	/*  setters  */
	void	set_path(std::string path);
	void	setBuffer(std::string *buffer);
	void	writeToBuffer(std::string info);
	void	adjustBuffer();
	bool	execute_response(int client_socket, Client *client);
	bool	GET_response(int client_socket, Client *client);
	bool	POST_response(int client_socket, Client *client);
	bool	DELETE_response(int client_socket, Client *client);
};

std::string	get_keyword(std::string req, std::string keyword);


#endif