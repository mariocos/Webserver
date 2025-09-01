#ifndef	RequestParse_HPP
# define RequestParse_HPP
# include "../../../includes/webserv.hpp"

class Client;
class Server;

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
	std::string	queryString;
	std::string	pathInfo;
	std::string	expect_something;
	std::string	*_buffer;
	int			error_code;
	std::string	content;
	std::vector<uint8_t>	_binaryBuffer;
	char *full_content;
	ssize_t	full_content_size;
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
	std::string	get_query_str();
	std::string	get_path_info();
	std::string get_content();
	std::string	get_connection();
	std::string	get_expect();
	char* get_full_content();
	void		buildRequest(const char *req);
	/*  setters  */
	void	set_path(std::string path);
	void	setBuffer(std::string *buffer);
	void	setNewHost(std::string str);
	void	setFullContent(char *req);
	void	addToFullContent(char *info, int len);
	ssize_t	getFullContentSize();
	void	readBinary(int client_socket, Client *client);
	void	clearBuffer();
	std::vector<uint8_t>	&getBufferInfo();
	void	execute_response(int client_socket, Client *client);
	void	GET_response(int client_socket, Client *client);
};

std::string	get_keyword(std::string req, std::string keyword);


#endif