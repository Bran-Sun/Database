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
#include "../Error/Error.h"

#define MAX_TABLE_NUM 32

struct DBHeadPage
{
    int tableNum;
    struct TableInfo{
        char tableName[TABLE_NAME_MAX_LENGTH + 1];
    } tables[MAX_TABLE_NUM];
};

class DatabaseHandle
{
public:
    DatabaseHandle();
    
    int open(const std::string &filename, std::shared_ptr<FileManager> fm, std::shared_ptr<BufPageManager> bpm);
    int createTable(const std::string &relName, std::vector<AttrInfo> &attributes);
    int dropTable(const std::string &relName);
    
    int createIndex(const std::string &relName, const std::string &attrName);
    int dropIndex(const std::string &relName, const std::string &attrName);
    
    //int load(std::string &relName, std::string &filename);
    
    bool isOpen() const { return _open; }
    
    int close();
    std::string getName() { if (_open) return _dbName; else return ""; }
    
    std::set<std::string> getTableInfo() { return _tableNames; }
    std::vector<AttrInfo> getRecordInfo(const std::string &tbName);
    void insert(const std::string &tbName, const std::vector<std::vector<DataAttr>> &data);
    void del(const std::string &tbName, std::vector<WhereClause> &whereClause);
    void update(const std::string &tbName, std::vector<WhereClause> &whereClause, std::vector<SetClause> &setClause);
    void select(std::vector<std::string> &tbList, std::vector<Col> &selector, bool selectAll, std::vector<WhereClause> &whereClause);
    
    ~DatabaseHandle();
    
public:
    void _modifyDBFile();
    bool _checkAttrInfo(const std::vector<AttrInfo> &attributes);
    void _openTable(const std::string tbName);

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
