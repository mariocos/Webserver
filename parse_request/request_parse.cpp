#include "request_parse.hpp"

request_parse::request_parse()
{
	std::cout << "Default request_parse constructor called\n";
}

#include <stdio.h>

request_parse::request_parse(char *request)
{
	std::cout << "calling request parse cstring constructor\n";
	if (!request)
	{
		std::cout << "im getting a null cstring\n";
		return ;//not sure what to do but im pretty sure this gets caught earlier
	}
	std::string	req(request);

	/* first line */
	req.find('\n');
	std::string line1 = req.substr(0, req.find('\n'));
	int	len = line1.find(' ');
	this->method = line1.substr(0, len);
	line1.erase(0, len + 1);
	len = line1.find(' ');
	this->path_to_request = line1.substr(0, len);
	line1.erase(0, len + 1);
	this->HTTP_version = line1.substr(0, line1.find('\r'));
	req.erase(0, req.find('\n') + 1);
	/*  line 2   */
	line1 = req.substr(0, req.find('\n'));
	this->Host = line1.substr(line1.find(' ') + 1, line1.find('\r') - (line1.find(' ') + 1));
	req.erase(0, req.find('\n') + 1);//plus 1 for after the newline
	/* line 3 */
	line1 = req.substr(0, req.find('\n'));
	this->User = line1.substr(line1.find(' ') + 1, line1.find('\r') - (line1.find(' ') + 1));
	req.erase(0, req.find('\n') + 1);
	/* line 4*/
	line1 = req.substr(0, req.find('\n'));
	this->Accepts = line1.substr(line1.find(' ') + 1, line1.find('\r') - (line1.find(' ') + 1));
}


request_parse::~request_parse()
{
}

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
