#include "RequestParse.hpp"


std::string	RequestParse::get_method(void)
{
	return (method);
}

std::string	RequestParse::get_host(void)
{
	return (Host);
}
std::string	RequestParse::get_httpversion(void)
{
	return (HTTP_version);
}
std::string	RequestParse::get_user(void)
{
	return (User);
}
std::string	RequestParse::get_accetps(void)
{
	return (Accepts);
}

std::string	RequestParse::get_path(void)
{
	return (path_to_request);
}

std::string	RequestParse::get_content_length(void)
{
	return (content_length);
}

std::string	RequestParse::get_content_type(void)
{
	return(content_type);
}

char*	RequestParse::get_full_content(void)
{
	return(full_content);
}