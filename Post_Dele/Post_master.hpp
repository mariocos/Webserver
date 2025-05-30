#ifndef POST_MASTER_HPP
#define POST_MASTER_HPP

#include "../includes/webserv.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <string>
#include <filesystem>

class Post_master
{
private:
    static std::string generateUniqueFilename(const std::string& directory, const std::string& extension);
    static void createDirectoryIfNeeded(const std::string& path);
    static bool isFileExists(const std::string& filename);
    static std::string getContentTypeExtension(const std::string& contentType);

public:
    static void post(Client** client);
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
    private:
        std::string message;
    public:
        FileOperationError(const std::string& msg) : message("File operation error: " + msg) {}
        const char* what() const throw() { return message.c_str(); }
    };
    
    class InvalidContentType : public std::exception
    {
    public:
        const char* what() const throw();
    };
};

Post_master::Post_master()
{
}

Post_master::~Post_master()
{
}


std::string Post_master::getContentTypeExtension(const std::string& contentType)
{
    // Map content types to file extensions
    if (contentType.find("text/plain") != std::string::npos) return ".txt";
    if (contentType.find("text/html") != std::string::npos) return ".html";
    if (contentType.find("application/json") != std::string::npos) return ".json";
    if (contentType.find("application/xml") != std::string::npos) return ".xml";
    if (contentType.find("image/jpeg") != std::string::npos) return ".jpg";
    if (contentType.find("image/png") != std::string::npos) return ".png";
    if (contentType.find("application/pdf") != std::string::npos) return ".pdf";
    
    // Default extension if content type is not recognized
    return ".dat";
}

std::string Post_master::generateUniqueFilename(const std::string& directory, const std::string& extension)
{
    // Generate a timestamp-based filename
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
    
    std::string baseFilename = std::string(buffer);
    std::string fullPath = directory + "/" + baseFilename + extension;
    
    // Ensure filename is unique by appending a counter if needed
    int counter = 1;
    while (isFileExists(fullPath)) {
        std::stringstream ss;
        ss << directory << "/" << baseFilename << "_" << counter << extension;
        fullPath = ss.str();
        counter++;
    }
    
    return fullPath;
}

bool Post_master::isFileExists(const std::string& filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

void Post_master::createDirectoryIfNeeded(const std::string& path)
{
    size_t pos = 0;
    std::string dir;
    
    while ((pos = path.find('/', pos + 1)) != std::string::npos) {
        dir = path.substr(0, pos);
        if (!dir.empty() && !isFileExists(dir)) {
            if (mkdir(dir.c_str(), 0755) != 0 && errno != EEXIST) {
                throw FileOperationError("Failed to create directory: " + dir);
            }
        }
    }
}

//temos de ver se aceitamos postarem com path ou so files na diretoria post
//TODO: check if file already exists
void Post_master::post(Client** client) // add socket
{
    if (!client || !*client) {
        throw BadClientPointer();//TODO: needs integration
    }
    
    // get request
    Request* request = (*client)->getClientRequest();
    if (!request) {
        throw EvilRequest();
    }
    

    // Get path from request
    std::string targetPath = request->get_path();
    if (targetPath.empty() || targetPath.find("..") != std::string::npos) {
        throw EvilRequest();
    }
    
    // Sanitize path to prevent directory traversal
    if (targetPath[0] == '/') {
        targetPath = targetPath.substr(1);
    }
    
	//adding prefix
	std::string result = "POSTED";
	if (request_path[0] != '/') {
		result += "/";
	}
	result += request_path;


    // TODO: when should this be done?
    createDirectoryIfNeeded(uploadDir);
    
	//TODO: write from buffer to file
	//TODO: get body from request and write to file;
}

#endif