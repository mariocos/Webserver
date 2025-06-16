#include "../includes/webserv.hpp"

const char* Post_master::EvilRequest::what() const throw()
{
    return "Evil request: The HTTP request is evil\n";
}

const char* Post_master::BadClientPointer::what() const throw()
{
    return "Bad client pointer: Null client pointer received\n";
}

const char* Post_master::BadRequestPointer::what() const throw()
{
    return "Bad Request pointer: Null Request pointer received\n";
}

const char* Post_master::FileAlreadyExists::what() const throw()
{
    return "File already exists\n";
}

const char* Post_master::InvalidContentType::what() const throw()
{
    return "Invalid content type: Content-Type header is missing or invalid\n";
}

const char* Post_master::FileOperationError::what() const throw()
{
    return "Invalid content type: Content-Type header is missing or invalid\n";
}


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
                throw FileOperationError();//revise
            }
        }
    }
}

//temos de ver se aceitamos postarem com path ou so files na diretoria post
//TODO: check if file already exists
void Post_master::post(Client* client) // add socket
{
    if (!client) {
        throw BadClientPointer();
    }
    
    // get request
    RequestParse* request = client->getClientRequest();
    if (!request) {
        throw BadRequestPointer();
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

	std::string result = "./POSTED";
	if (targetPath[0] != '/') {
		result += "/";
	}
	result += targetPath;//change name for readability


    // // TODO: when should this be done?
	createDirectoryIfNeeded("POSTED/");

	if (!access(result.c_str(), F_OK))
		throw Error409Exception(client->getSocketFd(), client->getClientResponse(), client);

	int out_fd = open(result.c_str(), O_WRONLY | O_CREAT);
	if (out_fd < 0)
		throw Error404Exception(client->getSocketFd(), client->getClientResponse(), client);
	write(out_fd, request->get_full_content(), atoi(request->get_content_length().c_str()));
	close(out_fd);
}
