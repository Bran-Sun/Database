//
// Created by 孙桢波 on 2018/12/6.
//

#ifndef DATABASE_SM_MANAGER_H
#define DATABASE_SM_MANAGER_H

#include "DatabaseHandle.h"
#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include <string>
#include <set>
#include <algorithm>

class SM_Manager
{
public:
    SM_Manager();
    ~SM_Manager();
    
    int createDb(const std::string &filename);
    int openDb(const std::string &filename, DatabaseHandle &handle);
    int closeDb(DatabaseHandle &handle);
    int destroyDb(const std::string &filename);
    
    int getDatabases(std::vector<std::string> &databaseName);
    
private:
    int _initDbManager();
    
private:
    std::shared_ptr<BufPageManager> _bpm;
    std::shared_ptr<FileManager> _fm;
    std::set<std::string> _dbs;
    bool _open;
};


#endif //DATABASE_SM_MANAGER_H
