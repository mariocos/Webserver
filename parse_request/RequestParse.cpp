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

bool	RequestParse::execute_response(int client_socket, Client &client)
{
	if (method == "GET")
		return (GET_response(client_socket, client));
	else if (method.compare("POST"))
		return (POST_response(client_socket, client));
	else if (method.compare("DELETE"))
		return (DELETE_response(client_socket, client));
	else
		std::cout<<"ERROR"<<std::endl;
	return (false);
}

bool	RequestParse::GET_response(int client_socket, Client &client)
{
	std::string	response, r_buffer, path, type = "text/plain";

	if (client.getClientPending() == false)
	{
		if (this->get_path() == "/")
			this->set_path("/dummy.html");
		if (this->get_path().length() > 5 && this->get_path().find(".html") == this->get_path().length() - 5)
			type = "text/html";
		else if (this->get_path().length() > 4 && this->get_path().find(".css") == this->get_path().length() - 4)
			type = "text/css";
		else if (this->get_path().length() > 4 && this->get_path().find(".png") == this->get_path().length() - 4)
			type = "image/png";
		else
			type = "text/html";
		response.append(this->get_httpversion() + " 200 OK\n");
		response.append("Content-Type: " + type + "\n");
		if (type != "image/png")
			response.append("Transfer-Encoding: chunked\n\n");
		else
			response.append("\n");
		path = "website" + this->get_path();
	}
	else
	{
		if (this->get_path() == "/")
			this->set_path("/dummy.html");
		if (this->get_path().length() > 5 && this->get_path().find(".html") == this->get_path().length() - 5)
			type = "text/html";
		else if (this->get_path().length() > 4 && this->get_path().find(".css") == this->get_path().length() - 4)
			type = "text/css";
		else
			type = "text/html";
		path = "website" + this->get_path();
	}
	if (type == "image/png")
	{
		std::ifstream	input;
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
	else
	{
		int	input;
		if (client.getClientOpenFd() == -1)
			input = open(path.c_str(), O_RDONLY);
		else
			input = client.getClientOpenFd();
		int	msgsize = 0;
		char	buffer[4096];
		size_t	bytes_read;
		std::string	lenght;
		std::ostringstream	number;
		if (input == -1)
		{
			input = open("website/404.html", O_RDONLY);
			bzero(buffer, sizeof(buffer));
			while ((bytes_read = read(input, buffer + msgsize, sizeof(buffer) - msgsize - 1)) > 0)
			{
				response.append(buffer);
				msgsize += bytes_read;
			}
			check(bytes_read);
			number<<msgsize;
			lenght = number.str();
			if (response.find("Transfer-Encoding: chunked") != std::string::npos)
				response.replace(response.find("Transfer-Encoding: chunked"), 27, "Content-lenght: " + lenght + "\n\n");
			write(client_socket, response.c_str(), response.length());
			close(client_socket);
			close(input);
		}
		else
		{
			bzero(buffer, sizeof(buffer));
			while ((bytes_read = read(input, buffer + msgsize, sizeof(buffer) - msgsize - 1)) > 0)
			{
				msgsize += bytes_read;
				response.append(buffer);
				if (msgsize >= 4095)
					break ;
			}
			check(bytes_read);
			if (msgsize >= 4095)
			{
				write(client_socket, response.c_str(), response.length());
				std::cout<<response<<std::endl;
				if (client.getClientOpenFd() == -1)
					client.setClientOpenFd(input);
				return (true);
			}
			number<<msgsize;
			lenght = number.str();
			if (response.find("Transfer-Encoding: chunked") != std::string::npos)
				response.replace(response.find("Transfer-Encoding: chunked"), 27, "Content-lenght: " + lenght + "\n\n");
			write(client_socket, response.c_str(), response.length());
			close(client_socket);
			close(input);
		}
	}
	return (false);
}

bool	RequestParse::POST_response(int client_socket, Client &client)
{
	(void)client_socket;
	return (client.getClientPending());
}

bool	RequestParse::DELETE_response(int client_socket, Client &client)
{
	(void)client_socket;
	return (client.getClientPending());
}
