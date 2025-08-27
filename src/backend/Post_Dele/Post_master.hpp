#ifndef POST_MASTER_HPP
#define POST_MASTER_HPP

#include "../../../includes/webserv.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <string>
#include <filesystem>

class Client;
class RequestParse;

class Post_master
{
private:
    std::string generateUniqueFilename(const std::string& directory, const std::string& extension);
    static void createDirectoryIfNeeded(const std::string& path);
    static bool isFileExists(const std::string& filename);
    std::string getContentTypeExtension(const std::string& contentType);

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
    class BadRequestPointer : public std::exception
    {
    public:
        const char* what() const throw();
    };    
    class FileAlreadyExists : public std::exception
    {
    public:
        const char* what() const throw();
    };
    class FileOperationError : public std::exception
    {
   		public:
        	const char* what() const throw();
    };
    
    class InvalidContentType : public std::exception
    {
    public:
        const char* what() const throw();
    };
};

int delete_resource(Client *client, RequestParse *req);

#endif