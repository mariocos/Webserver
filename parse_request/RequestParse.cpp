#include "RequestParse.hpp"

RequestParse::RequestParse()
{
	std::cout << "Default RequestParse constructor called\n";
}

RequestParse::RequestParse(const char *request)
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


RequestParse::~RequestParse()
{
	std::cout<<RED<<"Destructor"<<RESET<<std::endl;
}

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

void	RequestParse::set_path(std::string path)
{
	path_to_request = path;
}

bool	RequestParse::execute_response(int client_socket, Client *client)
{
	if (method.compare("GET") == 0)
		return (GET_response(client_socket, client));
	else if (method.compare("POST") == 0)
		return (POST_response(client_socket, client));
	else if (method.compare("DELETE") == 0)
		return (DELETE_response(client_socket, client));
	else
		std::cout<<"ERROR"<<std::endl;
	return (false);
}

bool	RequestParse::GET_response(int client_socket, Client *client)
{
	Response	response;

	if (client->getClientPending() == false)
	{
		if (this->get_path() == "/")
			this->set_path("/dummy.html");
		if (this->get_path().length() > 5 && this->get_path().find(".html") == this->get_path().length() - 5)
			response.setType("text/html");
		else if (this->get_path().length() > 4 && this->get_path().find(".css") == this->get_path().length() - 4)
			response.setType("text/css");
		else if (this->get_path().length() > 4 && this->get_path().find(".png") == this->get_path().length() - 4)
			response.setType("image/png");
		else
			response.setType("text/html");
		response.setResponse(response.getResponse().append(this->get_httpversion() + " 200 OK\n"));
		response.setResponse(response.getResponse().append("Content-Type: " + response.getType() + "\n"));
		if (response.getType() != "image/png")
			response.setResponse(response.getResponse().append("Transfer-Enconding: chunked\n\n"));
		else
			response.setResponse(response.getResponse().append("\n"));
		response.setPath("website" + this->get_path());
	}
	else
	{
		if (this->get_path() == "/")
			this->set_path("/dummy.html");
		if (this->get_path().length() > 5 && this->get_path().find(".html") == this->get_path().length() - 5)
			response.setType("text/html");
		else if (this->get_path().length() > 4 && this->get_path().find(".css") == this->get_path().length() - 4)
			response.setType("text/css");
		else
			response.setType("text/html");
		response.setPath("website" + this->get_path());
	}
	if (response.getType() == "image/png")
		return (loadImgResponse(client_socket, response));
	else
	{
		int	fd;
		if (client->getClientOpenFd() == -1)
			fd = open(response.getPath().c_str(), O_RDONLY);
		else
			fd = client->getClientOpenFd();
		if (fd == -1)
			return (loadErrorPage(client_socket, response));
		else
			return (loadPage(client_socket, fd, response, client));
	}
	return (false);
}

bool	RequestParse::POST_response(int client_socket, Client *client)
{
	(void)client_socket;
	return (client->getClientPending());
}

bool	RequestParse::DELETE_response(int client_socket, Client *client)
{
	(void)client_socket;
	return (client->getClientPending());
}
