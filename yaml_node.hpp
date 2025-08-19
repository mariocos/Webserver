#pragma once

# include <string>
# include <map>
# include <vector>
# include <iostream>
# include <fstream>
# include <cstdlib>
# include <sstream>
//# include "yaml_utils.hpp"

class YamlNode {
//    protected:
        bool _mapNode;
        bool _listNode;
    public:
        virtual ~YamlNode() {}
        void    setMap() {_mapNode = true; _listNode = false;}
        void    setList() {_mapNode = false; _listNode = true;}
        void    setScalar() {_mapNode = false; _listNode = false;}
        virtual bool    checkMap() {return _mapNode;}
        virtual bool    checkList() {return _listNode;}
        virtual void    print() const = 0;
};