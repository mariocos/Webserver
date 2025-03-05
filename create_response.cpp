#include "webserv.hpp"

bool	loadImgResponse(int client_socket, Response response)
{
	std::ifstream	input;
	std::string		buffer;
	input.open(response.getPath().c_str());
	if (input.is_open())
	{
		while (getline(input, buffer))
			response.setResponse(response.getResponse().append(buffer + "\n"));
	}
	else
	{
		input.open("website/404.html");
		while (getline(input, buffer))
			response.setResponse(response.getResponse().append(buffer + "\n"));
	}
	input.close();
	write(client_socket, response.getResponse().c_str(), response.getResponse().length());
	close(client_socket);
	return (false);
}

bool	loadErrorPage(int client_socket, Response response)
{
	std::ifstream	input;
	std::string	buffer;

	input.open("website/404.html");
	if (input.is_open())
	{
		while (getline(input, buffer))
			response.setResponse(response.getResponse().append(buffer + "\n"));
		input.close();
		write(client_socket, response.getResponse().c_str(), response.getResponse().length());
	}
	else
		std::cout<<"Error oppening the file"<<std::endl;
	close(client_socket);
	return (false);
}

int setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return (-1); // Error
    }
    return (fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}

bool	loadPage(int client_socket, int input, Response response, Client *client)
{
	int	msgsize = 0;
	char	buffer[4096];
	size_t	bytes_read;
	std::string	lenght;
	std::ostringstream	number;
	//std::ifstream	input;
	//FILE	*file;
	//std::string	buffer;
	//std::string	lenght;
	//std::ostringstream	number;

	bzero(buffer, sizeof(buffer));
	while ((bytes_read = read(input, buffer + msgsize, sizeof(buffer) - msgsize - 1)) > 0)
	{
		msgsize += bytes_read;
		response.setResponse(response.getResponse().append(buffer));
		if (msgsize >= 4095)
			break ;
	}
	check(bytes_read);
	if (msgsize >= 4095)
	{
		write(client_socket, response.getResponse().c_str(), response.getResponse().length());
		std::cout<<response.getResponse()<<std::endl;
		if (client->getClientOpenFd() == -1)
			client->setClientOpenFd(input);
		return (true);
	}
	number<<msgsize;
	lenght = number.str();
	if (response.getResponse().find("Transfer-Encoding: chunked") != std::string::npos)
		response.setResponse(response.getResponse().replace(response.getResponse().find("Transfer-Encoding: chunked"), 27, "Content-lenght: " + lenght + "\n\n"));
	write(client_socket, response.getResponse().c_str(), response.getResponse().length());
	close(client_socket);
	close(input);
	return (false);

	/* if (input.is_open())
	{
		while (getline(input, buffer))
		{
			response.setResponse(response.getResponse().append(buffer + "\n"));
			if (response.getResponse().size() >= 4095)
				break ;
		}
		input.close();
		if (response.getResponse().size() >= 4095)
		{
			write(client_socket, response.getResponse().c_str(), response.getResponse().length());
			std::cout<<response.getResponse()<<std::endl;
			if (client.getClientOpenFd() == -1)
				client.setClientOpenFd(fd);
			return (true);
		}
		number<<response.getResponse().size();
		lenght = number.str();
		if (response.getResponse().find("Transfer-Encoding: chunked") != std::string::npos)
			response.setResponse(response.getResponse().replace(response.getResponse().find("Transfer-Encoding: chunked"), 27, "Content-lenght: " + lenght + "\n\n"));
		write(client_socket, response.getResponse().c_str(), response.getResponse().length());
		close(client_socket);
		close(fd);
	}
	else
		return (loadErrorPage(client_socket, response));
	return (false); */
}
