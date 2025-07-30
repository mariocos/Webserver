#pragma once

# include <string>
# include <map>
# include <vector>
# include <iostream>
# include <fstream>
# include <cstdlib>
//# include "yaml_utils.hpp"

class YamlNode {
//    protected:
//        bool _mapNode;
//        bool _listNode;
    public:
        virtual ~YamlNode() {}
//        void    setMap() {_mapNode = true;}
//        void    setList() {_listNode = true;}
        virtual bool    isMap() = 0;/*{return _mapNode;}*/
        virtual bool    isList() = 0; /*{return _listNode;}*/
        virtual void    print() const = 0;
};