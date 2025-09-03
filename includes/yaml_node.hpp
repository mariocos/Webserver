#pragma once

# include <string>
# include <map>
# include <vector>
# include <iostream>
# include <fstream>
# include <cstdlib>
# include <sstream>
# include <climits>

//# include "yaml_utils.hpp"

class YamlNode {
//    protected:
        bool    _mapNode;
        bool    _listNode;
        bool    _scalarNode;
    public:
        virtual ~YamlNode() {}
        void    setMap() {_mapNode = true; _listNode = false; _scalarNode = false;}
        void    setList() {_mapNode = false; _listNode = true; _scalarNode = false;}
        void    setScalar() {_mapNode = false; _listNode = false; _scalarNode = true;}
        virtual bool    checkMap() {return _mapNode;}
        virtual bool    checkList() {return _listNode;}
        virtual bool    checkScalar() {return _scalarNode;}
        virtual void    print() const = 0;
};