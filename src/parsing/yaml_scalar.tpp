#pragma once

#include "../../includes/yaml_node.hpp"

// template <typename I>
// I	transformStringToSomething(const std::string &value)
// {
// 	int	check = returnVariableType(value);
// 	switch (check)
// 	{
// 		case 0:
// 			if (value == "true" || value == "on" || value == "yes")
// 				return (true);
// 			else
// 				return (false);
// 		case 1:
// 			return (std::atoi(value.c_str()));
// 		default:
// 			return (value);
// 	}
// }

template<typename T>
class YamlScalar : public YamlNode {
		T			_value;
		std::string	_type;
	
	public:
		YamlScalar();
		YamlScalar(const T, std::string type);
		//YamlScalar(std::string);
		//YamlScalar(int);
		//YamlScalar(bool);
		~YamlScalar() {}
		const T	&getValue() const {return _value;}
		void	setValue(const T value) {this->_value = value;}
		bool	isMap() {return this->checkMap();}
		bool	isList() {return this->checkList();}
		bool	isScalar() {return this->checkScalar();}
		void	print() const {std::cout << _value << std::endl;}
		std::string	getType() {return _type;}
};

template<typename T>
YamlScalar<T>::YamlScalar() {
	this->setScalar();
//	std::cout << _value << std::endl;
}

template<typename T>
YamlScalar<T>::YamlScalar(const T value, std::string type) {
	this->setScalar();
	_value = value;
	_type = type;
//	std::cout << _value << std::endl;
}
