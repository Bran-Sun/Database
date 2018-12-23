//
// Created by 孙桢波 on 2018/12/6.
//

#ifndef DATABASE_TABLEHANDLE_H
#define DATABASE_TABLEHANDLE_H

#include "../recordmanager/RM_Manager.h"
#include "../recordmanager/RM_FileHandle.h"
#include "../ixmanager/IX_Manager.h"
#include "../ixmanager/IX_IndexHandle.h"
#include "DataInfo.h"
#include <map>

class TableHandle
{
public:
    TableHandle();
    //open a table
    TableHandle(const std::string &dbName, const std::string &relName, std::shared_ptr<RM_Manager> rm, std::shared_ptr<IX_Manager> ix);
    //create a table
    TableHandle(const std::string &dbName, const std::string &relName, std::vector<AttrInfo> attributes, std::shared_ptr<RM_Manager> rm, std::shared_ptr<IX_Manager> ix);
    //drop a table;
    int dropTable();
    
    int createIndex(const std::string &attrName);
    int dropIndex(const std::string &attrName);
    int close();
    
    AttrInfo getPrimaryKey() const { return _primaryKey; }
    
private:
    void _openIndex();
    void _getPrimaryKey();
    
private:
    bool _open;
    AttrInfo _primaryKey;
    std::string _tableName, _dbName;
    std::vector<AttrInfo> _attributions;
    std::shared_ptr<RM_Manager> _rm;
    std::shared_ptr<IX_Manager> _ix;
    RM_FileHandle _rmHandle;
    std::map<std::string, IX_IndexHandle> _ixHandles;
};


#endif //DATABASE_TABLEHANDLE_H
