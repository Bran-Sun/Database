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
    std::string splitLine;
    splitLine.assign(25, '=');
    printf("%s\n", splitLine.c_str());
    printf("Table\n");
    printf("%s\n", splitLine.c_str());
    int count = 1;
    for (auto &name: tbNames) {
        printf("%d\t%s\n", count, name.c_str());
        count++;
    }
    printf("%s\n", splitLine.c_str());
}

void SystemManager::showDb()
{
    std::set<std::string> dbNames = _sm_manager.getDatabaseName();
    std::string splitLine;
    splitLine.assign(25, '=');
    printf("%s\n", splitLine.c_str());
    printf("Database\n");
    printf("%s\n", splitLine.c_str());
    int count = 1;
    for (auto name: dbNames) {
        printf("%d\t%s\n", count, name.c_str());
        count++;
    }
    printf("%s\n", splitLine.c_str());
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

void SystemManager::descTable(const std::string &tbName)
{
    std::vector<AttrInfo> attrs = _currentDb.getRecordInfo(tbName);
    std::string splitLine;
    splitLine.assign(25, '=');
    printf("%s\n", splitLine.c_str());
    printf("AttrName\tattrLength\tattrType\t\n");
    printf("%s\n", splitLine.c_str());
    for (auto &attr: attrs) {
        printf("%s\t%d\t%s\t\n", attr.attrName.c_str(), attr.attrLength, _getTypeString(attr).c_str());
    }
    printf("%s\n", splitLine.c_str());
}

std::string SystemManager::_getTypeString(const AttrInfo &attr)
{
    switch (attr.attrType) {
        case INT:
            return std::string("INT");
        case FLOAT:
            return std::string("FLOAT");
        case DATE:
            return std::string("DATE");
        case CHAR:
            return std::string("CHAR");
        case STRING:
            return std::string("STRING");
        case BOOL:
            return std::string("BOOL");
        default:
            return std::string("NUL");
    }
}

void SystemManager::insert(const std::string tbName, const std::vector<std::vector<DataAttr>> &data)
{
    auto first = data.begin() + 0;
    auto last = data.begin() + 100;
    std::vector<std::vector<DataAttr>> sub_data(first, last);
    _currentDb.insert(tbName, sub_data);
}

void SystemManager::del(const std::string tbName, std::vector<WhereClause> &whereClause)
{
    _currentDb.del(tbName, whereClause);
}

void SystemManager::update(const std::string tbName, std::vector<WhereClause> &whereClause,
                            std::vector<SetClause> &setClause)
{
    _currentDb.update(tbName, whereClause, setClause);
}

void SystemManager::select(std::vector<std::string> &tbList, std::vector<Col> &selector, bool selectAll,
                           std::vector<WhereClause> &whereClause)
{
    _currentDb.select(tbList, selector, selectAll, whereClause);
}
