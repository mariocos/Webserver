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
};





#endif