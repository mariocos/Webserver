#pragma once

#include "yaml_node.hpp"

class YamlList;

class YamlMap : public YamlNode {
		std::map<std::string, YamlNode*>	_map;
		size_t 								_index;
	public:
		YamlMap();
		~YamlMap();

		void								insert(const std::string&, YamlNode*);
//		void								yamlMapPrint();
		bool								isMap();
		bool								isList();
		bool								isScalar();
		YamlNode*							getMapNode(const std::string&) const;
		unsigned int						getIndex() const;
		std::map<std::string, YamlNode*>	&getMap();
		void								print() const;
};