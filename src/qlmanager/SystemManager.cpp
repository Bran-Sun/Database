//
// Created by 孙桢波 on 2018-12-22.
//

#include "SystemManager.h"

void SystemManager::showTable() {
    if (!_currentDb.isOpen()) {
        printf("no database is open\n");
        return;
    }
    
    std::set<std::string> tbNames = _currentDb.getTableInfo();
    printf("Table\n");
    for (auto &name: tbNames) {
        printf("%s\n", name.c_str());
    }
}

void SystemManager::showDb()
{
    std::set<std::string> dbNames = _sm_manager.getDatabaseName();
    printf("Database\n");
    for (auto name: dbNames) {
        printf("%s\n", name.c_str());
    }
}

void SystemManager::createTable(std::string &tbName, std::vector<AttrInfo> &attrInfo)
{
    _currentDb.createTable(tbName, attrInfo);
}

void SystemManager::destroyDb(std::string dbName)
{
    if (_currentDb.getName() == dbName) {
        _sm_manager.closeDb(_currentDb);
    }
    
    _sm_manager.destroyDb(dbName);
}

void SystemManager::descTable(const std::string tbName)
{

}

void SystemManager::insert(const std::string tbName, const std::vector<std::vector<DataAttr>> &data)
{

}

void SystemManager::del(const std::string tbName, const std::vector<WhereClause> &whereClause)
{

}

void SystemManager::update(const std::string tbName, const std::vector<WhereClause> &whereClause,
                           const std::vector<SetClause> &setClause)
{

}

void SystemManager::select(const std::vector<std::string> &tbList, const std::vector<Col> &seletor, bool selectAll,
                           const std::vector<WhereClause> whereClause)
{

}
