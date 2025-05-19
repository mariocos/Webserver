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
	//std::string		_buffer;
	std::vector<char>	_buffer;
	bool			_checkingSize;
	bool			_isReading;
	bool			_isWriting;
public:
	File();
	File(Client *client);
	~File();
	std::fstream	*getFile();
	struct stat			*getFileStats();
	Client			*getClient();
	//std::string		readFromBuffer();
	std::vector<char>	&readFromBuffer();
	bool			getCheckingSizeFlag();
	bool			isReading();
	bool			isWriting();
	unsigned int	getBytesRead();
	void			setClient(Client *client);
	void			setCheckingSizeFlag(bool flag);
	void			setReading(bool flag);
	void			setWriting(bool flag);
	void			clearBuffer();
	void			writeToBuffer(char *info, size_t len);
	void			adjustBuffer();
	void			readFromFd(unsigned int buffer_size);
	void			openFile(const char *path, int client_socket);
	bool			checkFileInfo(const char *path, int client_socket);
};

#endif