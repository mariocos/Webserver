#include "yaml_list.hpp"

YamlList::YamlList() {
//	this->setList();
//	std::cout << "LIST" << std::endl;
}

YamlList::~YamlList() {
	std::vector<YamlNode*>::iterator	it;
	for (it = _list.begin(); it != _list.end(); it++) {
		delete *it;
		*it = NULL;
	}
	_list.clear();
}

void		YamlList::add(YamlNode* node) {
	_list.push_back(node);
//	std::cout << node << std::endl;
}

YamlNode	*YamlList::get(size_t index) const {
	if (index >= _list.size())
		return NULL;
	return _list[index];
}

size_t		YamlList::size() const {
	return _list.size();
}

bool	YamlList::isMap() {
	return (false);
}

bool	YamlList::isList() {
	return (true);
}

std::vector<YamlNode*>	&YamlList::getList() {
	return _list;
}

void	YamlList::print() const {
	std::vector<YamlNode*>::const_iterator it;
    for (it = _list.begin(); it != _list.end(); ++it) {
//        for (int i = 0; i < indent; ++i)
//            std::cout << "\t";
        std::cout << "- ";
        (*it)->print();
    }
}