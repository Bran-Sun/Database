//
// Created by 孙桢波 on 2018/12/6.
//

#include "TableHandle.h"

TableHandle::TableHandle()
{
    _open = false;
}

TableHandle::TableHandle(const std::string &relName, std::shared_ptr<RM_Manager> rm, std::shared_ptr<IX_Manager> ix)
{
    _rm = rm;
    _ix = ix;
    
    _tableName = relName;
    _rm->openFile(_tableName, _rmHandle);
    _attributions = _rmHandle.getAttrInfo();
    
    _openIndex();
    _open = true;
}

TableHandle::TableHandle(const std::string &relName, std::vector<AttrInfo> attributes, std::shared_ptr<RM_Manager> rm,
                         std::shared_ptr<IX_Manager> ix)
{
    _tableName = relName;
    _rm = rm;
    _ix = ix;
    
    _attributions  = attributes;
    _rm->createFile(_tableName, _attributions); //create
    
    _rm->openFile(_tableName, _rmHandle);
    //index is empry!
    
    _open = true;
}

int TableHandle::dropTable()
{
    _rm->closeFile(_rmHandle);
    
    for (auto iter = _ixHandles.begin(); iter != _ixHandles.end(); iter++) {
        _ix->closeIndex(iter->second);
    }
    
    _rm->destroyFile(_tableName);
    for (int i = 0; i < _attributions.size(); i++) {
        if (_attributions[i].isIndex) {
            _ix->destroyIndex(_tableName.c_str(), i);
        }
    }
    
    _ixHandles.clear();
    _attributions.clear();
    
    _open = false;
    
    return 0;
}

int TableHandle::createIndex(std::string &attrName)
{
    auto find = _ixHandles.find(attrName);
    if (find != _ixHandles.end()) {
        printf("tbHandle: index has already created!\n");
        return -1;
    }
    
    _ixHandles.emplace(std::piecewise_construct,
                       std::forward_as_tuple(attrName),
                       std::forward_as_tuple());
    
    int indexNo;
    for (indexNo = 0; indexNo < _attributions.size(); indexNo++)
        if  (_attributions[indexNo].attrName == attrName) {
            break;
        }
    _ix->createIndex(_tableName.c_str(), indexNo, _attributions[indexNo].attrType, _attributions[indexNo].attrLength);
    _ix->openIndex(_tableName.c_str(), indexNo, _ixHandles.at(attrName));
    
    _rmHandle.addIndex(indexNo);
    _attributions[indexNo].isIndex = true;
    
    return 0;
}

int TableHandle::dropIndex(std::string &attrName)
{
    auto find =  _ixHandles.find(attrName);
    if (find == _ixHandles.end()) {
        printf("tbHandle: table doesn't have this index!\n");
        return -1;
    }
    
    int indexNo;
    for (indexNo = 0; indexNo < _attributions.size(); indexNo++)
        if (_attributions[indexNo].attrName == attrName) {
            break;
        }
    
    _ix->closeIndex(_ixHandles.at(attrName));
    _ix->destroyIndex(_tableName.c_str(), indexNo);
    _ixHandles.erase(attrName);
    
    _attributions[indexNo].isIndex = false;
    _rmHandle.dropIndex(indexNo);
    
    return 0;
}

int TableHandle::close()
{
    _rm->closeFile(_rmHandle);
    
    for (auto iter = _ixHandles.begin(); iter != _ixHandles.end(); iter++) {
        _ix->closeIndex(iter->second);
    }
    
    _ixHandles.clear();
    _attributions.clear();
    
    _open = false;
    return 0;
}

void TableHandle::_openIndex()
{
    for (int i = 0; i < _attributions.size(); i++) {
        if (_attributions[i].isIndex) {
            _ixHandles.emplace(std::piecewise_construct,
                               std::forward_as_tuple(_attributions[i].attrName),
                               std::forward_as_tuple());
            _ix->openIndex(_tableName.c_str(), i, _ixHandles.at(_attributions[i].attrName));
        }
    }
}

