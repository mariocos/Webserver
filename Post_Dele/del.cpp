#include "../includes/webserv.hpp"

int delete_resource(RequestParse req)
{
    // Sanitize path to prevent directory traversal
    std::string targetPath = req.get_path();
    if (targetPath.empty() || targetPath.find("..") != std::string::npos) {
        return (-1); // Evil request
    }
    
    // Remove leading slash if present
    if (targetPath[0] == '/') {
        targetPath = targetPath.substr(1);
    }
    
    // Construct full path with POSTED prefix (matching your POST implementation)
    std::string fullPath = "./POSTED";
    if (targetPath[0] != '/') {
        fullPath += "/";
    }
    fullPath += targetPath;
    
    // Check if file/resource exists
    if (access(fullPath.c_str(), F_OK) != 0) {
        std::cout << "Resource does not exist: " << fullPath << std::endl;
        return (-2); // Not found
    }
    
    // Check if it's a directory
    struct stat pathStat;
    if (stat(fullPath.c_str(), &pathStat) != 0) {
        std::cout << "Cannot stat resource: " << fullPath << std::endl;
        return (-3);
    }
    
    if (S_ISDIR(pathStat.st_mode)) {
        // It's a directory - remove it (only if empty)
        if (rmdir(fullPath.c_str()) != 0) {
            std::cout << "Cannot remove directory (not empty or permission denied): " << fullPath << std::endl;
            return (-4);
        }
        std::cout << "Directory deleted successfully: " << fullPath << std::endl;
    } else {
        // It's a regular file - delete it
        if (unlink(fullPath.c_str()) != 0) {
            std::cout << "Cannot delete file (permission denied): " << fullPath << std::endl;
            return (-5);
        }
        std::cout << "File deleted successfully: " << fullPath << std::endl;
    }
    
    return (1); // Success
}