#pragma once

#include "yaml_node.hpp"

class YamlList : public YamlNode {
		std::vector<YamlNode*>	_list;

	public:
		YamlList();
		~YamlList();

		void					add(YamlNode*);
		YamlNode				*get(size_t) const;
		std::vector<YamlNode*>	&getList();
		size_t					size() const;
		bool    				isMap();
		bool    				isList();
		bool    				isScalar();
//		void					yamlListPrint();
		void					print() const;
};