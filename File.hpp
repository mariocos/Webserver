#ifndef FILE_HPP
# define FILE_HPP

# include "webserv.hpp"

class Client;

class File
{
private:
	int	_fd;
	unsigned int	_bytesRead;
	Client *_client;
	std::string	*_buffer;
	bool	_checkingSize;
	bool	_isReading;
	bool	_isWriting;
public:
	File();
	File(int fd, Client *client);
	~File();
	int	getFd();
	Client	*getClient();
	std::string	readFromBuffer();
	bool	getCheckingSizeFlag();
	bool	isReading();
	bool	isWriting();
	unsigned int	getBytesRead();
	void	setFd(int fd);
	void	setClient(Client *client);
	void	setCheckingSizeFlag(bool flag);
	void	setReading(bool flag);
	void	setWriting(bool flag);
	void	clearBuffer();
	void	writeToBuffer(char *info);
	void	adjustBuffer();
	void	readFromFd();
};

#endif