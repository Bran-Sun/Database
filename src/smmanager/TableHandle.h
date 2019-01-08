//
// Created by 孙桢波 on 2018/12/6.
//

#ifndef DATABASE_TABLEHANDLE_H
#define DATABASE_TABLEHANDLE_H

#include "../recordmanager/RM_Manager.h"
#include "../recordmanager/RM_FileHandle.h"
#include "../recordmanager/RM_Iterator.h"
#include "../ixmanager/IX_Manager.h"
#include "../ixmanager/IX_IndexHandle.h"
#include "../ixmanager/IX_IndexScan.h"
#include "DataInfo.h"
#include "../Error/Error.h"
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
    std::vector<AttrInfo> getAttributions() const { return _attributions; }
    
    void insert(const std::vector<std::vector<DataAttr>> &data);
    void insert(const std::vector<DataAttr> &data);
    void del(std::vector<WhereClause> &whereClause);
    void del(RM_Record &record);
    void update(std::vector<WhereClause> &whereClause, std::vector<SetClause> &setClause);
    void selectSingle(std::vector<Col> &selector, bool selectAll, std::vector<WhereClause> &whereClause);
    
    bool addForeign(const std::string &key);
    bool delForeign(const std::string &key);
    
    bool checkWhereValid(std::vector<WhereClause> &whereClause);
    bool getWhereRecords(std::vector<WhereClause> &whereClause, std::vector<RM_Record> &records);

private:
    void _openIndex();
    void _getPrimaryKey();
    bool _checkWhereClause(RM_Record &record, std::vector<WhereClause> &whereClause);
    bool _checkSetValid(std::vector<SetClause> &setClause);
    void _modifyWhereClause(std::vector<WhereClause> &whereClause);
    
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
