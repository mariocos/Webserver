#include "../includes/webserv.hpp"

int delete_resource(Client *client, RequestParse *req)
{
    // Sanitize path to prevent directory traversal
    std::string targetPath = req->get_path();
    if (targetPath.empty() || targetPath.find("..") != std::string::npos) {
        return (-1); // Evil request
    }
    
    // Remove leading slash if present
    if (targetPath[0] == '/') {
        targetPath = targetPath.substr(1);
    }
    
    // Construct full path with POSTED prefix (matching your POST implementation)
	std::string	uploadPath = client->getRouteTriggered()->getUploadPath();
	if (uploadPath.empty())
		uploadPath = "./POSTED";
    if (targetPath[0] != '/') {
		uploadPath += "/";
    }
	uploadPath += targetPath;
    
    // Check if file/resource exists
    if (access(uploadPath.c_str(), F_OK) != 0) {
		throw Error404Exception(client->getSocketFd(), client->getClientResponse(), client);
    }
    
    // Check if it's a directory
    struct stat pathStat;
    if (stat(uploadPath.c_str(), &pathStat) != 0) {
        std::cout << "Cannot stat resource: " << uploadPath << std::endl;
        return (-3);
    }
    
    if (S_ISDIR(pathStat.st_mode)) {
        // It's a directory - remove it (only if empty)
        if (rmdir(uploadPath.c_str()) != 0) {
			throw Error409Exception(client->getSocketFd(), client->getClientResponse(), client);
        }
    } else {
        // It's a regular file - delete it
        if (unlink(uploadPath.c_str()) != 0) {
			throw Error403Exception(client->getSocketFd(), client->getClientResponse(), client);
        }
    }
    
    return (1);
}