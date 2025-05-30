const char* Post_master::EvilRequest::what() const throw()
{
    return "Evil request: The HTTP request contains malicious or invalid content\n";
}

const char* Post_master::BadClientPointer::what() const throw()
{
    return "Bad client pointer: Null client pointer received\n";
}

const char* Post_master::InvalidContentType::what() const throw()
{
    return "Invalid content type: Content-Type header is missing or invalid\n";
}
