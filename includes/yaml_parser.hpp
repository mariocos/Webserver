#pragma once

#include "yaml_utils.hpp"

struct LineToken {
	int			indent;
	std::string	key;
	std::string	value;
	bool		isListItem;
};

class YamlParser {
		std::vector<LineToken>	tokenizer(const std::string &);
		YamlNode				*parseMapBlock(std::vector<LineToken> &, size_t &, int);
		YamlNode				*parseListBlock(std::vector<LineToken> &, size_t &, int);
	public:
		YamlNode	*parse(const std::string &);
};