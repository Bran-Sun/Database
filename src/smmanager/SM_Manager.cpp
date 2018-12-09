//
// Created by 孙桢波 on 2018/12/6.
//

#include "SM_Manager.h"
#include <unistd.h>
#include <sys/types.h>

SM_Manager::SM_Manager()
{
    _fm = std::make_shared<FileManager>(FileManager());
    _bpm = std::make_shared<BufPageManager>(BufPageManager(_fm));
    _open = false;
    _initDbManager();
}

SM_Manager::~SM_Manager()
{

}

int SM_Manager::createDb(const std::string &filename)
{
    if (access(filename.c_str(), 0) == 0) {
        printf("sm_error: database already exist!\n");
        return -1;
    }
    
    if (mkdir(filename.c_str(), S_IRWXU) != 0) {
        printf("sm_error: failed to create database!\n");
        return -1;
    }
    
    _dbs.emplace(filename);
    std::string dbFilename = filename + "/" + filename + ".dbf";
    _fm->createFile(dbFilename.c_str());
    int fileID;
    _fm->openFile(dbFilename.c_str(), fileID);
    int pageID = 0, index;
    BufType headerPage = _bpm->allocPage(fileID, pageID, index, false); //初始为空
    _bpm->markDirty(index);
    _bpm->writeBack(index);
    _fm->closeFile(fileID);
    return 0;
}

int SM_Manager::openDb(const std::string &filename, DatabaseHandle &handle)
{
    if (_dbs.count(filename) == 0) {
        printf("sm_error: database not exits!\n");
        return -1;
    }

    int result = handle.open(filename, _fm, _bpm);
    if (result == -1) {
        printf("sm_error: database cannot open!\n");
        return -1;
    }
    
    _open = true;
    return 0;
}

int SM_Manager::closeDb(DatabaseHandle &handle)
{
    handle.close();
    _open = false;
    return 0;
}

int SM_Manager::getDatabases(std::vector<std::string> &databaseName)
{
    databaseName.clear();
    
    for (auto iter = _dbs.begin(); iter != _dbs.end(); iter++) {
        databaseName.emplace_back(*iter);
    }
    return 0;
}

int SM_Manager::_initDbManager()
{
    auto curDir = opendir(".");
    if (curDir == nullptr) {
        printf("sm_error: cannot open current directory!\n");
        return -1;
    }
    
    _dbs.clear();
    
    dirent* d;
    d = readdir(curDir);
    while (d != nullptr) {
        std::string subName = d->d_name;
        subName = subName + "/" + subName + ".dbf";
        if (access(subName.c_str(), 0) == 0) {
            _dbs.emplace(std::string(d->d_name));
        }
        d = readdir(curDir);
    }
    return 0;
}

int SM_Manager::destroyDb(const std::string &filename)
{
    if (!_open) {
        printf("sm_error: no database is open!\n");
    }
    
    if (_dbs.count(filename) == 0) {
        printf("sm_error: database is not exist!\n");
        return -1;
    }
    
    if (rmdir(filename.c_str()) != 0) {
        printf("sm_error: delete database error!\n");
        return -1;
    }
    
    _dbs.erase(filename);
    return 0;
}
