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
        this->foreignTb = std::string(r.foreignTb);
        this->foreignIndex = std::string(r.foreignIndex);
        this->attrLength  = r.attrLength;
        this->attrType = r.attrType;
        
        this->isNull = r.isNull;
        this->isIndex = r.isIndex;
        this->isPrimary = r.isPrimary;
        this->isForeign = r.isForeign;
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

struct Col{
    std::string tbName, indexName;
    
    Col() {}
    Col(std::string t, std::string i): tbName(t), indexName(i) {}
};

struct WhereVal{
    bool isVal;
    bool useNull;
    bool isNull;
    Col col;
    AttrType type;
    std::string value;
    
    WhereVal() {
        isNull = false;
        useNull = false;
    }
};

struct SetClause{
    std::string value, col;
    AttrType attrType;
    
    SetClause() {}
    SetClause(std::string c, std::string v, AttrType t) : col(c), value(v), attrType(t) {}
};

struct WhereClause{
    WhereVal left, right;
    ComOp comOp;
};

struct DataAttr{
    std::string data;
    AttrType attrType;
    bool isNull;
    
    DataAttr() {
        isNull = false;
    }
    
    DataAttr(std::string d, AttrType type): data(d), attrType(type) { isNull = false; }
    DataAttr(bool null): isNull(null) {}
};

#endif //DATABASE_DATAINFO_H
