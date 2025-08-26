#pragma once

#include "yaml_node.hpp"

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
		~YamlScalar();
		const T	&getValue() const;
		void	setValue(const T);
		bool	isMap() {return false;}
		bool	isList() {return false;}
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

template<typename T>
YamlScalar<T>::~YamlScalar() {}

template<typename T>
const T	&YamlScalar<T>::getValue() const {
	return _value;
}

template<typename T>
void	YamlScalar<T>::setValue(const T value) {
	this->_value = value;
}
