#ifndef POST_MASTER_HPP
#define POST_MASTER_HPP

#include "../includes/webserv.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <string>
#include <filesystem>

class Client;

class Post_master
{
private:
    static std::string generateUniqueFilename(const std::string& directory, const std::string& extension);
    static void createDirectoryIfNeeded(const std::string& path);
    static bool isFileExists(const std::string& filename);
    static std::string getContentTypeExtension(const std::string& contentType);

public:
    static void post(Client* client);
    Post_master();
    ~Post_master();

    // Exception classes
    class EvilRequest : public std::exception
    {
    public:
        const char* what() const throw();
    };
    
    class BadClientPointer : public std::exception
    {
    public:
        const char* what() const throw();
    };
    
    class FileOperationError : public std::exception
    {
   		public:
        	const char* what() const throw();
    	// private:
    	//     std::string message;
    	// public:
    	//     FileOperationError(const std::string& msg) : message("File operation error: " + msg) {}
    	//     const char* what() const throw() { return message.c_str(); }
    };
    
    class InvalidContentType : public std::exception
    {
    public:
        const char* what() const throw();
    };
};


#endif