#include "RequestParse.hpp"

RequestParse::RequestParse()
{
	std::cout << "Default RequestParse constructor called\n";
}

RequestParse::RequestParse(const char *request)
{
	error_code = 0;
	std::cout << "calling request parse cstring constructor\n";
	if (!request)
	{
		std::cout << "im getting a null cstring\n";
		return ;//not sure what to do but im pretty sure this gets caught earlier
	}
	std::string	req(request);
	/* first line */
	std::string line1 = req.substr(0, req.find('\n'));
	int	len = line1.find(' ');
	this->method = line1.substr(0, len);
	line1.erase(0, len + 1);
	len = line1.find(' ');
	this->path_to_request = line1.substr(0, len);
	if (path_to_request.find("../"))
	{
		std::cout << "naughty request\n";
		error_code = 3;
	}
	line1.erase(0, len + 1);
	this->HTTP_version = line1.substr(0, line1.find('\r'));
	req.erase(0, req.find('\n') + 1);
	/* key word get */
	content_length = get_keyword(req, "content-length: ");
	content_type = get_keyword(req, "content-type: ");
	User = get_keyword(req, "User: ");
	Accepts = get_keyword(req, "Accept: ");
}


std::string	get_keyword(std::string req, std::string keyword)
{
	int	i = 0;

	if (!req.find(keyword))
		return ("");
	i = req.find(keyword) + keyword.length();
	while (req[i] != '\r')
		i++;
	return (req.substr(req.find(keyword) + keyword.length(), i));
}

RequestParse::~RequestParse()
{
}

void	RequestParse::set_path(std::string path)
{
	path_to_request = path;
}

void	RequestParse::execute_response(int client_socket)
{
	if (method == "GET")
		GET_response(client_socket);
	else if (method.compare("POST"))
		POST_response(client_socket);
	else if (method.compare("DELETE"))
		DELETE_response(client_socket);
	else
		std::cout<<"ERROR"<<std::endl;
}

void	RequestParse::GET_response(int client_socket)
{
	std::string	response, r_buffer, path, type = "text/plain";
	std::ifstream	input;

	if (this->get_path() == "/")
		this->set_path("/dummy.html");
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
