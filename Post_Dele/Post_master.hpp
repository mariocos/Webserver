#ifndef POST_MASTER_HPP
#define POST_MASTER_HPP

#include "../includes/webserv.hpp"

class Post_master
{
private:
	/* data */
	

public:

	static void post(Client* client);
	Post_master(/* args */);
	~Post_master();

	class EvilRequest : public std::exception
	{
		public:
			const char	*what(void)const throw(); 
	};
	class BadClientPointer : public std::exception
	{
		public:
			const char	*what(void)const throw(); 
	};
};

Post_master::Post_master(/* args */)
{
}

Post_master::~Post_master()
{
}

const char *Post_master::EvilRequest::what() const  throw()
{
	return("Evil_request\n");
}
const char *Post_master::BadClientPointer::what() const  throw()
{
	return("BadClientPointer\n");
}

void	Post_master::post(Client* client)
{
	if (!client)
		throw (BadClientPointer());
	client->getClientRequest()->get_path();
}









#endif