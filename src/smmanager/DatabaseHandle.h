//
// Created by 孙桢波 on 2018/12/6.
//

#ifndef DATABASE_DATABASEHANDLE_H
#define DATABASE_DATABASEHANDLE_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include "DataInfo.h"
#include "../utils/pagedef.h"
#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "../recordmanager/RM_Manager.h"
#include "../ixmanager/IX_Manager.h"
#include "TableHandle.h"

struct DBHeadPage
{
    int tableNum;
    struct TableInfo{
        char tableName[TABLE_NAME_MAX_LENGTH + 1];
    } tables[MAX_COL_NUM];
};

class DatabaseHandle
{
public:
    DatabaseHandle();
    
    int open(const std::string &filename, std::shared_ptr<FileManager> fm, std::shared_ptr<BufPageManager> bpm);
    int createTable(const std::string &relName, std::vector<AttrInfo> attributes);
    int dropTable(const std::string &relName);
    
    int createIndex(const std::string &relName, const std::string &attrName);
    int dropIndex(const std::string &relName, const std::string &attrName);
    
    //int load(std::string &relName, std::string &filename);
    
    int close();
    
    ~DatabaseHandle();
    
public:
    void _modifyDBFile();

private:
    bool _modifyDbf;
    std::shared_ptr<FileManager> _fm;
    std::shared_ptr<BufPageManager> _bpm;
    std::shared_ptr<RM_Manager> _rm;
    std::shared_ptr<IX_Manager> _ix;
    std::set<std::string> _tableNames;
    std::string _dbName;
    std::map<std::string, TableHandle> _tableHandles;
    int _fileID; //id for dbf file
    bool _open;
};


#endif //DATABASE_DATABASEHANDLE_H
