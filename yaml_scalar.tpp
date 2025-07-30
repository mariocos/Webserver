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
		T	_value;
	
	public:
		YamlScalar();
		YamlScalar(const T);
		//YamlScalar(std::string);
		//YamlScalar(int);
		//YamlScalar(bool);
		~YamlScalar();
		const T	&getValue() const;
		void	setValue(const T);
		bool	isMap() {return false;}
		bool	isList() {return false;}
		void	print() const {std::cout << _value << std::endl;}
};

template<typename T>
YamlScalar<T>::YamlScalar() {
//	std::cout << _value << std::endl;
}

template<typename T>
YamlScalar<T>::YamlScalar(const T value) {
	_value = value;
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
