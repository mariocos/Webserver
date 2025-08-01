#include "yaml_map.hpp"

YamlMap::YamlMap() : _index(0) {
//	this->setMap();
//	std::cout << "MAP" << std::endl;
}

YamlMap::~YamlMap() {
	std::map<std::string, YamlNode*>::iterator it;
	for (it = _map.begin(); it != _map.end(); ++it) {
		delete it->second;
		it->second = NULL;
	}
	_map.clear();
}

void	YamlMap::insert(const std::string& key, YamlNode* value) {
	_index = _map.size() + 1;
	_map[key] = value;
//	std::cout << key << std::endl;
}

YamlNode	*YamlMap::getMapNode(const std::string& key) const {
	std::map<std::string, YamlNode*>::const_iterator it = _map.find(key);
	if (it != _map.end())
		return it->second;
	return NULL;
}

std::map<std::string, YamlNode*>	&YamlMap::getMap() {
	return _map;
}

unsigned int	YamlMap::getIndex() const {
	return _index;
}

bool	YamlMap::isMap() {
	return (true);
}

bool	YamlMap::isList() {
	return (false);
}

void	YamlMap::print() const {
	std::map<std::string, YamlNode*>::const_iterator it;
    for (it = _map.begin(); it != _map.end(); ++it) {
//        for (int i = 0; i < indent; ++i)
//            std::cout << "\t";
        std::cout << it->first << ": ";
        if (it->second) {
            // Scalars inline, others on new line
//            YamlScalar* scalar = dynamic_cast<YamlScalar*>(it->second);
            if (it->second->isMap() || it->second->isList()) {
                std::cout << std::endl;
            }
			it->second->print();
        }
        else {
            std::cout << "null" << std::endl;
        }
    }
}