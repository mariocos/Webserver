#ifndef FILE_HPP
# define FILE_HPP

# include "webserv.hpp"

class Client;

class File
{
private:
	std::fstream	_file;
	struct stat		_fileStats;
	unsigned int	_bytesRead;
	Client 			*_client;
	std::vector<uint8_t>	_buffer;
	bool			_isReading;
	bool			_isWriting;
public:
	File();
	File(Client *client);
	~File();
	std::fstream	*getFile();
	struct stat			*getFileStats();
	Client			*getClient();
	std::vector<uint8_t>	&readFromBuffer();
	bool			isReading();
	bool			isWriting();
	unsigned int	getBytesRead();
	void			setClient(Client *client);
	void			setReading(bool flag);
	void			setWriting(bool flag);
	void			clearBuffer();
	void			readFromFd(unsigned int buffer_size);
	void			openFile(const char *path, int client_socket);
	void			closeFile();
	bool			checkFileInfo(const char *path, int client_socket);
};

#endif