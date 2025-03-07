#ifndef	RequestParse_HPP
# define RequestParse_HPP
# include "../webserv.hpp"

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
	std::string	content;
	std::string	content_length;
	std::string	content_type;
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
	/*  setters  */
	void	set_path(std::string path);
	void	execute_response(int client_socket);
	void	GET_response(int client_socket);
	void	POST_response(int client_socket);
	void	DELETE_response(int client_socket);
};

std::string	get_keyword(std::string req, std::string keyword);


#endif