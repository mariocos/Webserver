#include "yaml_utils.hpp"

bool	checkIntYaml(const std::string &str, size_t len)
{
	size_t	n = 0;
	bool	signal = false;
	if (str[n] == '-' || str[n] == '+')
	{
		signal = true;
		n++;
	}
	while (n < len)
	{
		if (!isdigit(str[n]))
			break ;
		n++;
	}
	if (n == len && ((signal == true && len < 11) || (signal == false && len < 10)))
		return (true);
	return (false);
}

bool    checkYamlNode(const std::string &value)
{
    size_t pos = value.find(":");
    if ((*value.begin()) == '[') {
        if ((*value.end()) == ']')
            return true;
        else {
            std::cerr << "Inline list not correctly ended." << std::endl;
			return false;
		}
    }
    else if (pos == value.length())
        return true;
    else
        return false;
}

/*
	0 -> Bool
	1 -> Int
	2 -> YamlNode
	3 -> std::string
 */

int		yamlReturnVariableType(const std::string &value)
{
	if (value == "true" || value == "false" || value == "on" || value == "off" || value == "yes" || value == "no")
		return (0);
	else if (checkIntYaml(value, value.length()))
		return (1);
	else
		return (2);
}

bool	transformStringToBool(const std::string &value)
{
	if (value == "true" || value == "on" || value == "yes")
		return (true);
	else
		return (false);
}

int	transformStringToInt(const std::string &value)
{
	return (std::atoi(value.c_str()));
}

//void	yamlMapP(YamlMap *node) {
//	node->yamlMapPrint();
//}

//void	yamlListP(YamlList *node) {
//	node->yamlListPrint();
//}

/*void	yamlListPrint(std::vector<YamlNode*> list) {
	std::vector<YamlNode*>::const_iterator	it;
	for (it = list.begin(); it != list.end(); it++) {
		if (it[0]->isMap())
			yamlMapP(dynamic_cast<YamlMap*>(it[0]));
		else if (it[0]->isList())
			yamlListPrint();
		else
			std::cout << *it << std::endl;
	}
	if (it == list.end()){
		if (it[0]->isMap())
			yamlMapP(dynamic_cast<YamlMap*>(it[0]));
		else if (it[0]->isList())
			yamlListPrint();
		else
			std::cout << *it << std::endl;
	}
}

void	yamlMapPrint(std::map<std::string, YamlNode*> map) {
	std::map<std::string, YamlNode*>::const_iterator it;
	for (it = map.begin(); it != map.end(); it++) {
		if (it->second->isMap())
			yamlMapPrint();
		else if (it->second->isList())
			yamlListP(dynamic_cast<YamlList*>(it->second));
		else
			std::cout << it->second << std::endl;
	}
	if (it == map.end()){
		if (it->second->isMap())
			yamlMapPrint();
		else if (it->second->isList())
			yamlListP(dynamic_cast<YamlList*>(it->second));
		else
			std::cout << it->second << std::endl;
	}
}*/

// template<typename T>
// YamlScalar<T>::YamlScalar(bool value) {
// 	_value = value;
// }

// template<typename T>
// YamlScalar<T>::YamlScalar(int value) {
// 	_value = value;
// }

// template<typename T>
// YamlScalar<T>::YamlScalar(std::string value) {
// 	_value = value;
// }
