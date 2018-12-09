#include <utility>

//
// Created by 孙桢波 on 2018/12/6.
//

#include "DatabaseHandle.h"

DatabaseHandle::DatabaseHandle()
{
    _bpm = nullptr;
    _fm = nullptr;
    _rm = nullptr;
    _ix = nullptr;
    _fileID = -1;
    _open = false;
    _modifyDbf = false;
}

int DatabaseHandle::close()
{
    if (_modifyDbf) _modifyDBFile();
    
    for (auto iter = _tableHandles.begin(); iter != _tableHandles.end(); iter++) {
        iter->second.close();
    }
    
    _fm->closeFile(_fileID);
    
    _tableHandles.clear();
    _tableNames.clear();
    _fm = nullptr;
    _bpm = nullptr;
    _rm = nullptr;
    _ix = nullptr;
    _fileID = -1;
    _open = false;
    _dbName = "";
    _modifyDbf = false;
    return 0;
}

int DatabaseHandle::open(const std::string &filename, std::shared_ptr<FileManager> fm, std::shared_ptr<BufPageManager> bpm)
{
    if (_dbName == filename) {
        printf("dbhandle: database is already open!\n");
        return 1;
    }
    close(); //open another
    
    _dbName = filename;
    _fm = fm;
    _bpm = bpm;
    _rm = std::make_shared<RM_Manager>(RM_Manager(_fm, _bpm));
    _ix = std::make_shared<IX_Manager>(IX_Manager(_fm, _bpm));
    
    std::string dbname = filename + "/" + filename + ".dbf";
    
    int pageID, index;
    
    if (_fm->openFile(dbname.c_str(), _fileID) == -1) {
        printf("dbhanlde: cannot open dbf file!\n");
        return -1;
    }
    
    //load dbf file
    pageID = 0;
    BufType header = _bpm->allocPage(_fileID, pageID, index, true);
    DBHeadPage *headerPage = (DBHeadPage*)header;
    int tableNum = headerPage->tableNum;
    for (int i = 0; i < tableNum; i++) {
        _tableNames.emplace(headerPage->tables[i].tableName);
    }
    _bpm->release(index);
    _open = true;
    
    return 0;
}

void DatabaseHandle::_modifyDBFile()
{
    int pageID = 0, index;
    BufType header = _bpm->allocPage(_fileID, pageID, index);
    DBHeadPage *headerPage = (DBHeadPage*)header;
    headerPage->tableNum = (int)_tableNames.size();
    int cnt = 0;
    for (auto &i: _tableNames) {
        memcpy(headerPage->tables[cnt].tableName, i.c_str(), i.size() + 1);
        cnt++;
    }
    _bpm->markDirty(index);
    _bpm->writeBack(index);
    _bpm->release(index);
}

int DatabaseHandle::createTable(const std::string &relName, std::vector<AttrInfo> attributes)
{
    if (!_open) {
        printf("dbHandle: database bot open!\n");
        return -1;
    }
    
    auto find = _tableNames.find(relName);
    if (find != _tableNames.end()) {
        printf("dbHandle: database already exists!\n");
        return -1;
    }
    
    _tableHandles.emplace(std::piecewise_construct,
                          std::forward_as_tuple(relName),
                          std::forward_as_tuple(relName, _rm, _ix));
    _tableNames.emplace(relName);
    return 0;
}

int DatabaseHandle::dropTable(const std::string &relName)
{
    auto find = _tableNames.find(relName);
    if (find == _tableNames.end()) {
        printf("dbHandle: database not exists!\n");
        return -1;
    }
    
    _tableHandles.at(relName).dropTable();
    _tableNames.erase(relName);
    
    return 0;
}

int DatabaseHandle::createIndex(std::string &relName, std::string &attrName)
{
    auto find = _tableNames.find(relName);
    if (find == _tableNames.end()) {
        printf("dbHandle: database not exists!\n");
        return -1;
    }
    
    auto openFind = _tableHandles.find(relName);
    if (openFind == _tableHandles.end()) {
        _tableHandles.emplace(std::piecewise_construct,
                              std::forward_as_tuple(relName),
                              std::forward_as_tuple(relName, _rm, _ix));
    }
    int result = _tableHandles.at(relName).createIndex(attrName);
    
    return result;
}

int DatabaseHandle::dropIndex(std::string &relName, std::string &attrName)
{
    auto find = _tableNames.find(relName);
    if (find == _tableNames.end()) {
        printf("dbHandle: database not exists!\n");
        return -1;
    }
    
    auto openFind = _tableHandles.find(relName);
    if (openFind == _tableHandles.end()) {
        _tableHandles.emplace(std::piecewise_construct,
                              std::forward_as_tuple(relName),
                              std::forward_as_tuple(relName, _rm, _ix));
    }
    int result = _tableHandles.at(relName).dropIndex(attrName);
    return result;
}

/* not use
int DatabaseHandle::load(std::string &relName, std::string &filename)
{
    auto find = _tableNames.find(relName);
    if (find == _tableNames.end()) {
        printf("dbHandle: database not exists!\n");
        return -1;
    }
    
    auto openFind = _tableHandles.find(relName);
    if (openFind == _tableHandles.end()) {
        _tableHandles.emplace(relName, relName, _rm, _ix);
    }
    int result = _tableHandles.at(relName)->loadData(filename);
    
    return result;
}*/

DatabaseHandle::~DatabaseHandle()
{
    close();
}
