#include "yaml_parser.hpp"

static int	countIndent(const std::string &str) {
	int count = 0;
	for (size_t i = 0; str.length() && str[i] == ' '; ++i)
		count++;
	return count;
}

static std::string	trim(const std::string &str) {
	size_t	start = 0;
	while (start < str.size() && std::isspace(str[start]))
		start++;
	size_t end = str.size();
	while (end > start && std::isspace(str[end - 1]))
		end--;
	return (str.substr(start, end - start));
}

static bool isInLineArray(const std::string &str) {
	std::string trimmed = trim(str);
	return trimmed.length() >= 2 && trimmed[0] == '[' && trimmed[trimmed.length() - 1] == ']';
}

static YamlList	*parseInLineArray(const std::string &array) {
	YamlList	*list = new YamlList();
	
	std::string content = trim(array);
	if (content.length() >= 2 && content[0] == '[' && content[content.length()-1] == ']') {
		content = content.substr(1, content.length() - 2);
		content = trim(content);

		if (!content.empty()) {
			size_t	start = 0;
			size_t	pos = 0;

			while (pos < content.length()) {
				if (content[pos] == ',') {
					std::string item = trim(content.substr(start, pos - start));
					if (!item.empty()) {
						int check = yamlReturnVariableType(item);
						switch (check) {
							case 0:
								list->add(new YamlScalar<bool>(transformStringToBool(item)));
								break;
							case 1:
								list->add(new YamlScalar<int>(transformStringToInt(item)));
								break;
							default:
								list->add(new YamlScalar<std::string>(item));
						}
					}
					start = pos + 1;
				}
				pos++;
			}
			std::string lastItem = trim(content.substr(start));
			if (!lastItem.empty()) {
				int check = yamlReturnVariableType(lastItem);
				switch (check) {
					case 0:
						list->add(new YamlScalar<bool>(transformStringToBool(lastItem)));
						break;
					case 1:
						list->add(new YamlScalar<int>(transformStringToInt(lastItem)));
						break;
					default:
						list->add(new YamlScalar<std::string>(lastItem));
//					list->add(new YamlScalar(transformStringToSomething(lastItem)));
				}
			}
		}
	}
	return list;
}

std::vector<LineToken>	YamlParser::tokenizer(const std::string &filePath) {
	std::ifstream			file(filePath.c_str());
	std::string				line;
	std::vector<LineToken>	tokens;

	if (!file.is_open()) {
		std::cout << "Error: Could not open file " << filePath << std::endl;
		return tokens;
	}

	while (std::getline(file, line)) {
		if (line.empty() || trim(line).empty())
			continue;
		LineToken	lineTokens;
		lineTokens.indent = countIndent(line);
		std::string	trimmedLine = trim(line);

		lineTokens.isListItem = false;
		lineTokens.key = "";
		lineTokens.value = "";

		if (trimmedLine[0] == '-') {
			lineTokens.isListItem = true;
			lineTokens.indent += 2;
			trimmedLine = trim(trimmedLine.substr(1));
			size_t	pos = trimmedLine.find(':');
			if (pos != std::string::npos) {
				lineTokens.key = trim(trimmedLine.substr(0, pos));
				lineTokens.value = trim(trimmedLine.substr(pos + 1));
			}
			else
				lineTokens.value = trimmedLine;
		}
		else {
			size_t	pos = trimmedLine.find(':');
			if (pos != std::string::npos) {
				lineTokens.key = trim(trimmedLine.substr(0, pos));
				lineTokens.value = trim(trimmedLine.substr(pos + 1));
			}
			else {
				lineTokens.key = trimmedLine;
				lineTokens.value = "";
			}
		}
		tokens.push_back(lineTokens);
	}
	return tokens;
}

YamlNode	*YamlParser::parseMapBlock(std::vector<LineToken> &lines, size_t &index, int curIndent) {
	YamlMap	*map = new YamlMap();

	while (index < lines.size()) {
		LineToken	&token = lines[index];

		if (token.indent < curIndent)
			return map;
		if (token.indent > curIndent) {
			std::cerr << "Error: Unexpected indentation in map block at line " << index << std::endl;
			return map;
		}
		if (token.isListItem) {
            std::cerr << "Error: Unexpected list item in map block at line " << index << std::endl;
            index++;
            continue; //?? not sure is needed;
		}

		if (token.value.empty()) {
			std::string currentKey = token.key;
			index++;

			if(index < lines.size() && lines[index].isListItem && lines[index].indent == curIndent + 4) {
				YamlNode	*nestedList = parseListBlock(lines, index, curIndent + 4);
				map->insert(currentKey, nestedList);
			}
			else if (index < lines.size() && !lines[index].isListItem && lines[index].indent == curIndent + 2) {
				YamlNode	*nestedMap = parseMapBlock(lines, index, curIndent + 4);
				map->insert(currentKey, nestedMap);
			}
			else 
				map->insert(currentKey, new YamlScalar<std::string>(""));
		}
		else {
			if (isInLineArray(token.value)) {
				YamlList	*inLineList = parseInLineArray(token.value);
				map->insert(token.key, inLineList);
			}
			else {
				int check = yamlReturnVariableType(token.value);
				switch (check) {
					case 0:
						map->insert(token.key, new YamlScalar<bool>(transformStringToBool(token.value)));
						break;
					case 1:
						map->insert(token.key, new YamlScalar<int>(transformStringToInt(token.value)));
						break;
					default:
						map->insert(token.key, new YamlScalar<std::string>(token.value));
				}
			}
//				map->insert(token.key, new YamlScalar(transformStringToSomething(token.value)));
			index++;
		}
	}
	return map;
}

YamlNode	*YamlParser::parseListBlock(std::vector<LineToken> &lines, size_t &index, int curIndent) {
	YamlList	*list = new YamlList();

	while (index < lines.size()) {
		LineToken &token = lines[index];

		if (!token.indent || token.indent < curIndent)
			return list;
		if (token.indent > curIndent) {
			std::cerr << "Error: Unexpected indentation in list block at line " << index << std::endl;
			return list;
		}
		if (!token.key.empty()) {
			YamlMap	*listItemMap = new YamlMap();

			if (isInLineArray(token.value)) {
				YamlList	*inLineList = parseInLineArray(token.value);
				listItemMap->insert(token.key, inLineList);
			}
			else {
				int check = yamlReturnVariableType(token.value);
				switch (check) {
					case 0:
						listItemMap->insert(token.key, new YamlScalar<bool>(transformStringToBool(token.value)));
						break;
					case 1:
						listItemMap->insert(token.key, new YamlScalar<int>(transformStringToInt(token.value)));
						break;
					default:
						listItemMap->insert(token.key, new YamlScalar<std::string>(token.value));
				}
//				listItemMap->insert(token.key, new YamlScalar(transformStringToSomething(token.value)));
			}
			index++;
			while (index < lines.size() && lines[index].indent == curIndent) {
				if (!lines[index].isListItem) {
					LineToken	&childToken = lines[index];

					if (childToken.value.empty()) {
						std::string	childKey = childToken.key;
						index++;
						
						if (index < lines.size()) {
							if (lines[index].isListItem && lines[index].indent == curIndent + 4) {
								YamlNode	*nestedList = parseListBlock(lines, index, curIndent + 4);
								listItemMap->insert(childKey, nestedList);
							}
							else if (!lines[index].isListItem && lines[index].indent == curIndent + 2) {
								YamlNode	*nestedMap = parseMapBlock(lines, index, curIndent + 2);
								listItemMap->insert(childKey, nestedMap);
							}
							else
								listItemMap->insert(childKey, new YamlScalar<std::string>(""));
						}
						else 
							listItemMap->insert(childKey, new YamlScalar<std::string>(""));
					}
					else {
						if (isInLineArray(childToken.value)) {
							YamlList	*inLineList = parseInLineArray(childToken.value);
							listItemMap->insert(childToken.key, inLineList);
						}
						else {
							int check = yamlReturnVariableType(childToken.value);
							switch (check) {
								case 0:
									listItemMap->insert(childToken.key, new YamlScalar<bool>(transformStringToBool(childToken.value)));
									break;
								case 1:
									listItemMap->insert(childToken.key, new YamlScalar<int>(transformStringToInt(childToken.value)));
									break;
								default:
									listItemMap->insert(childToken.key, new YamlScalar<std::string>(childToken.value));
							}
						}
//							listItemMap->insert(childToken.key, new YamlScalar(transformStringToSomething(childToken.value)));
						index++;
					}
				}
				else
					break;
			}
			list->add(listItemMap);
		}
		else {
			int check = yamlReturnVariableType(token.value);
			switch (check) {
				case 0:
					list->add(new YamlScalar<bool>(transformStringToBool(token.value)));
					break;
				case 1:
					list->add(new YamlScalar<int>(transformStringToInt(token.value)));
					break;
				default:
					list->add(new YamlScalar<std::string>(token.value));
			}
//			list->add(new YamlScalar(transformStringToSomething(token.value)));
			index++;
		}
	}
	return list;
}

YamlNode	*YamlParser::parse(const std::string &file_path) {
	std::vector<LineToken> tokens = tokenizer(file_path);
	size_t	index = 0;

	if (tokens.empty())
		return NULL;

	if (tokens[0].isListItem && tokens[0].indent == 0)
		return parseListBlock(tokens, index, 0);
	else if (!tokens[0].isListItem && tokens[0].indent == 0)
		return parseMapBlock(tokens, index, 0);
	else
		std::cout << "ERROR: Indentarion." << std::endl;
	return NULL;
}