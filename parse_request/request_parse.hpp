#ifndef	REQUEST_PARSE_HPP
#define REQUEST_PARSE_HPP
# include <iostream>







class request_parse
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
	request_parse();
	request_parse(char *request);
	~request_parse();
	/*  getters  */
	std::string	get_method();
	std::string	get_host();
	std::string	get_httpversion();
	std::string	get_user();
	std::string	get_accetps();
	std::string	get_path();
};

std::string	request_parse::get_method(void)
{
	return (method);
}

std::string	request_parse::get_host(void)
{
	return (Host);
}
std::string	request_parse::get_httpversion(void)
{
	return (HTTP_version);
}
std::string	request_parse::get_user(void)
{
	return (User);
}
std::string	request_parse::get_accetps(void)
{
	return (Accepts);
}

std::string	request_parse::get_path(void)
{
	return (path_to_request);
}

#endif