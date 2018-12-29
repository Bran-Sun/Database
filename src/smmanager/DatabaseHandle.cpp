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
    if (!_open) {
        printf("database is already close!\n");
        return -1;
    }
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
    
    if (_open)
    {
        close(); //open another
    }
    
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

bool DatabaseHandle::_checkAttrInfo(const std::vector<AttrInfo> &attributes)
{
    for (const auto &info: attributes) {
        if (info.attrName.length() > ATTRNAME_MAX_LEN) {
            return false;
        }
        
        if (info.isForeign) {
            if (_tableNames.find(info.foreignTb) == _tableNames.end()) {  //如果不存在这个表
                return false;
            }
            
            if (_tableHandles.find(info.foreignTb) == _tableHandles.end()) {    //打开这个表
                _tableHandles.emplace(std::piecewise_construct,
                                      std::forward_as_tuple(info.foreignTb),
                                      std::forward_as_tuple(_dbName, info.foreignTb, _rm, _ix));
            }
            
            AttrInfo p = _tableHandles.at(info.foreignTb).getPrimaryKey();
            if (p.attrName != info.foreignIndex || p.attrType != info.attrType || p.attrLength != info.attrLength) {
                return false;
            }
        }
    }
    return true;
}

int DatabaseHandle::createTable(const std::string &relName, std::vector<AttrInfo> &attributes)
{
    //first process the attributions
    for (auto &at: attributes) {
        if (at.isPrimary || at.isForeign) {
            at.isIndex = true;
        }
    }
    
    if (!_open) {
        printf("dbHandle: table bot open!\n");
        return -1;
    }
    
    auto find = _tableNames.find(relName);
    if (find != _tableNames.end()) {
        printf("dbHandle: table already exists!\n");
        return -1;
    }
    
    if (!_checkAttrInfo(attributes)) {
        printf("dbHandle: attributes format error(maybe foreign key)\n");
        return -1;
    }
    
    _tableHandles.emplace(std::piecewise_construct,
                          std::forward_as_tuple(relName),
                          std::forward_as_tuple(_dbName, relName, attributes, _rm, _ix));
    _tableNames.emplace(relName);
    _modifyDbf = true;
    return 0;
}

int DatabaseHandle::dropTable(const std::string &relName)
{
    if (!_open) {
        printf("database not open!\n");
        return -1;
    }
    
    auto find = _tableNames.find(relName);
    if (find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return -1;
    }
    
    _tableHandles.at(relName).dropTable();
    _tableHandles.erase(relName);
    _tableNames.erase(relName);
    _modifyDbf = true;
    return 0;
}

int DatabaseHandle::createIndex(const std::string &relName, const std::string &attrName)
{
    if (!_open) {
        printf("dbHandle: database not open\n");
        return -1;
    }
    
    auto find = _tableNames.find(relName);
    if (find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return -1;
    }
    
    auto openFind = _tableHandles.find(relName);
    if (openFind == _tableHandles.end()) {
        _tableHandles.emplace(std::piecewise_construct,
                              std::forward_as_tuple(relName),
                              std::forward_as_tuple(_dbName, relName, _rm, _ix));
    }
    int result = _tableHandles.at(relName).createIndex(attrName);
    
    return result;
}

int DatabaseHandle::dropIndex(const std::string &relName, const std::string &attrName)
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
                              std::forward_as_tuple(_dbName, relName, _rm, _ix));
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
    if (_open) close();
}

std::vector<AttrInfo> DatabaseHandle::getRecordInfo(const std::string &tbName)
{
    auto find = _tableNames.find(tbName);
    if (find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return std::vector<AttrInfo>();
    }
    
    auto openFind = _tableHandles.find(tbName);
    if (openFind == _tableHandles.end()) {
        _tableHandles.emplace(std::piecewise_construct,
                              std::forward_as_tuple(tbName),
                              std::forward_as_tuple(_dbName, tbName, _rm, _ix));
    }
    
    return _tableHandles.at(tbName).getAttributions();
}

void DatabaseHandle::insert(const std::string &tbName, const std::vector<std::vector<DataAttr>> &data) {
    auto find = _tableNames.find(tbName);
    if (find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return;
    }
    
    auto openFind = _tableHandles.find(tbName);
    if (openFind == _tableHandles.end()) {
        _tableHandles.emplace(std::piecewise_construct,
                              std::forward_as_tuple(tbName),
                              std::forward_as_tuple(_dbName, tbName, _rm, _ix));
    }
    
    _tableHandles.at(tbName).insert(data);
}

void DatabaseHandle::del(const std::string &tbName, std::vector<WhereClause> &whereClause) {
    auto find = _tableNames.find(tbName);
    if (find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return;
    }
    
    auto openFind = _tableHandles.find(tbName);
    if (openFind == _tableHandles.end()) {
        _tableHandles.emplace(std::piecewise_construct,
                              std::forward_as_tuple(tbName),
                              std::forward_as_tuple(_dbName, tbName, _rm, _ix));
    }
    
    _tableHandles.at(tbName).del(whereClause);
}