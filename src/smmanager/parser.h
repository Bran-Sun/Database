//
// Created by 孙桢波 on 2018/12/6.
//

#ifndef DATABASE_PARSER_H
#define DATABASE_PARSER_H

#include "../utils/pagedef.h"
#include "../recordmanager/PageHeaderFile.h"
#include <string>

struct AttrInfo {
    std::string attrName;
    int attrLength;
    AttrType attrType;
    bool isNull, isIndex;
    
    AttrInfo(AttrInfoRead r) {
        this->attrName = std::string(r.attrName);
        this->attrLength  = r.attrLength;
        this->attrType = r.attrType;
        this->isNull = r.isNull;
        this->isIndex = r.isIndex;
    }
};

struct DataAttrInfo {
    std::string relName;
    std::string attrName;
    int offset;
    AttrType attrType;
    int attrLength;
    int indexNo;
};

#endif //DATABASE_PARSER_H
