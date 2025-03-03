#include "ConfigParser.hpp"


ConfigParser::ConfigParser()
{
}

ConfigParser::ConfigParser(const char *file)
{
	std::ifstream	infile;
	std::string		buffer, configStr;
	infile.open(file);
	if (infile.is_open())
	{
		while (getline(infile, buffer))
			configStr.append(buffer);
		infile.close();
		configStr.find("listen");
	}
	else
		std::cout<<"There was an error opening the .config file"<<std::endl;
}

ConfigParser::ConfigParser(const ConfigParser &copy){}

ConfigParser::~ConfigParser(){}

ConfigParser&	ConfigParser::operator=(const ConfigParser &copy)
{
	return (*this);
}
