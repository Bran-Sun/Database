//
// Created by 孙桢波 on 2018-12-22.
//

#ifndef DATABASE_SYSTEMMANAGER_H
#define DATABASE_SYSTEMMANAGER_H

#include "../smmanager/SM_Manager.h"

class SystemManager
{
public:
    SystemManager() {}
    
    void createDb(std::string dbName) { _sm_manager.createDb(dbName); }  //ok
    
    void createTable(std::string &tbName, std::vector<AttrInfo> &attrInfo);  //ok
    void createIndex(const std::string &tbName, const std::string &indexName) { _currentDb.createIndex(tbName, indexName); };  //ok
    
    void destroyDb(std::string dbName); //ok
    void destroyTable(const std::string tbName) { _currentDb.dropTable(tbName); } //ok
    void destroyIndex(const std::string tbName, const std::string indexName) { _currentDb.dropIndex(tbName, indexName); } //ok
    
    void showDb(); //ok
    void showTable();   //ok
    
    void useDb(std::string dbName) { _sm_manager.openDb(dbName, _currentDb); } //ok
    
    void descTable(const std::string tbName);
    void insert(const std::string tbName, const std::vector<std::vector<DataAttr>> &data);
    void del(const std::string tbName, const std::vector<WhereClause> &whereClause);
    void update(const std::string tbName, const std::vector<WhereClause> &whereClause, const std::vector<SetClause> &setClause);
    void select(const std::vector<std::string> &tbList, const std::vector<Col> &seletor, bool selectAll, const std::vector<WhereClause> whereClause);
    
    ~SystemManager() { if (_currentDb.isOpen()) _sm_manager.closeDb(_currentDb); }
    
private:
    SM_Manager _sm_manager;
    DatabaseHandle _currentDb;
};


#endif //DATABASE_SYSTEMMANAGER_H
