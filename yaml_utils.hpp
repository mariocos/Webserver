#pragma once
 
# include "yaml_node.hpp"
# include "yaml_list.hpp"
# include "yaml_map.hpp"
# include "yaml_scalar.tpp"

bool	checkIntYaml(const std::string &str, size_t len);
bool	checkYamlNode(const std::string &value);
int		yamlReturnVariableType(const std::string &value);
bool	transformStringToBool(const std::string &value);
int		transformStringToInt(const std::string &value);
//void	yamlMapP(YamlMap *node);
//void	yamlListP(YamlList *node);
//void	yamlListPrint(std::vector<YamlNode*> list);
//void	yamlMapPrint(std::map<std::string, YamlNode*> map);