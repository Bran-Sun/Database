//
// Created by 孙桢波 on 2018/12/6.
//

#include "DatabaseHandle.h"

DatabaseHandle::DatabaseHandle()
{
    _bpm = nullptr;
    _fm = nullptr;
    _fileID = -1;
    _open = false;
    _modifyDbf = false;
}

int DatabaseHandle::close()
{
    if (_modifyDbf) _modifyDBFile();
    
    for (auto iter = _tableHandles.begin(); iter != _tableHandles.end(); iter++) {
        iter->second->close();
    }
    
    _fm->closeFile(_fileID);
    ß
    _tableHandles.clear();
    _tableNames.clear();
    _fm = nullptr;
    _bpm = nullptr;
    _fileID = -1;
    _open = false;
    _dbName = "";
    _modifyDbf = false;
    return 0;
}

int DatabaseHandle::open(std::string &filename, std::shared_ptr<FileManager> fm, std::shared_ptr<BufPageManager> bpm)
{
    if (_dbName == filename) {
        printf("dbhandle: database is already open!\n");
        return 1;
    }
    close(); //open another
    
    _dbName = filename;
    _fm = fm;
    _bpm = bpm;
    
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
    
    return 0;
}

void DatabaseHandle::_modifyDBFile()
{
    int pageID = 0, index;
    BufType header = _bpm->allocPage(_fileID, pageID, index);
    DBHeadPage *headerPage = (DBHeadPage*)header;
    headerPage->tableNum = _tableNames.size();
    int cnt = 0;
    for (auto i: _tableNames) {
        memcpy((headerPage->tables[cnt].tableName, i.c_str(), i.size());
        headerPage->tables[cnt].tableName[i.size()] = '\0';
        cnt++;
    }
    _bpm->markDirty(index);
    _bpm->writeBack(index);
    _bpm->release(index);
}
