//
// Created by 孙桢波 on 2018/12/6.
//

#ifndef DATABASE_DATAINFO_H
#define DATABASE_DATAINFO_H

#include "../utils/pagedef.h"
#include "../recordmanager/PageHeaderFile.h"
#include <string>

struct AttrInfo {
    std::string attrName, foreignTb, foreignIndex;
    int attrLength;
    AttrType attrType;
    bool isNull, isIndex, isPrimary, isForeign;
    
    AttrInfo(AttrInfoRead r) {
        this->attrName = std::string(r.attrName);
        this->attrLength  = r.attrLength;
        this->attrType = r.attrType;
        this->isNull = r.isNull;
        this->isIndex = r.isIndex;
        //TODO
        this->isPrimary = false;
        this->isForeign = false;
    }
    
    AttrInfo(std::string name, int length, AttrType type) {
            attrName = name;
            attrLength = length;
            attrType = type;
            isNull = false;
            isIndex = false;
            isPrimary = false;
            isForeign = false;
    }
    
    AttrInfo() {
            attrLength = 0;
            isNull = false;
            isIndex = false;
            isPrimary = false;
            isForeign = false;
    }
};

struct WhereVal{
    bool isVal;
    std::string tbName, indexName;
    AttrType type;
    std::string value;
    int attrLength;
};

struct WhereClause{
    WhereVal left, right;
    ComOp comOp;
};

struct DataAttr{
    std::string data;
    AttrType attrType;
    int attrLength;
};

#endif //DATABASE_DATAINFO_H
