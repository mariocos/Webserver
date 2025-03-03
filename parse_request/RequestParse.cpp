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

void	RequestParse::execute_response(int client_socket)
{
	int i;
	if (method == "GET")
		i = 1;
	else if (method == "POST")
		i = 2;
	else if (method == "DELETE")
		i = 3;
	else
		i = -1;

	switch (i)
	{
	case 1:
		GET_response(client_socket);
		break;
	case 2:
		POST_response(client_socket);
		break;
	case 3:
		DELETE_response(client_socket);
		break;
	default:
		break;
	}
}

void	RequestParse::GET_response(int client_socket)
{
	std::string	response, r_buffer, path, type = "text/plain";
	std::ifstream	input;

	if (this->get_path() == "/")
		this->set_path("/index.html");
	if (this->get_path().length() > 5 && this->get_path().find(".html") == this->get_path().length() - 5)
		type = "text/html";
	else if (this->get_path().length() > 4 && this->get_path().find(".css") == this->get_path().length() - 4)
		type = "text/css";
	else
		type = "text/html";
	response.append(this->get_httpversion() + " 200 OK\n");
	response.append("Content-Type: " + type + "\n\n");
	path = "website" + this->get_path();
	input.open(path.c_str());
	if (input.is_open())
	{
		while (getline(input, r_buffer))
		{
			response.append(r_buffer);
			response.append("\n");
		}
	}
	else
	{
		input.open("website/404.html");
		while (getline(input, r_buffer))
		{
			response.append(r_buffer);
			response.append("\n");
		}
	}
	input.close();
	write(client_socket, response.c_str(), response.length());
	close(client_socket);
}

void	RequestParse::POST_response(int client_socket)
{
	(void)client_socket;
}

void	RequestParse::DELETE_response(int client_socket)
{
	(void)client_socket;
}
