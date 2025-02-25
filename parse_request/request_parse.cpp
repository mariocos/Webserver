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
	/*  line 2   */
	req.erase(0, req.find('\n') + 1);
	line1 = req.substr(0, req.find('\n'));
	this->Host = line1.substr(line1.find(' ') + 1, line1.find('\r'));
	std::cout << "host take away " << (Host == "localhost:4243") << "[" << Host << "]\n";


}


request_parse::~request_parse()
{
}


