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
	std::string	content_type;
	std::string	content_length;
	std::string	connection;
	std::string	*_buffer;
	int			error_code;
	std::string	content;
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
	std::string	get_buffer();
	void		buildRequest(const char *req);
	/*  setters  */
	void	set_path(std::string path);
	void	setBuffer(std::string *buffer);
	void	writeToBuffer(char *info);
	void	adjustBuffer();
	void	readToBuffer(int client_socket, Client *client);
	void	execute_response(int client_socket, Client *client);
	void	GET_response(int client_socket, Client *client);
	void	POST_response(int client_socket, Client *client);
	void	DELETE_response(int client_socket, Client *client);
};

std::string	get_keyword(std::string req, std::string keyword);


#endif