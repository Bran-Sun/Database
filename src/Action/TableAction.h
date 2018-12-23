//
// Created by 孙桢波 on 2018-12-22.
//

#ifndef DATABASE_TABLEACTION_H
#define DATABASE_TABLEACTION_H

#include "Action.h"

class DescTable : public Action
{
public:
    DescTable(std::string tbName): _tbName(tbName) {}
    void execute(SystemManager &system) {
        system.descTable(_tbName);
    }
    void show() {
        printf("desc table %s\n", _tbName.c_str());
    }
private:
    std::string _tbName;
};

class InsertTable : public Action
{
public:
    InsertTable(std::string tbName, std::vector<std::vector<DataAttr>> &data): _tbName(tbName), _data(data) {}
    void execute(SystemManager &system) {
        system.insert(_tbName, _data);
    }
    void show() {
        printf("insert table %s\n", _tbName.c_str());
    };
private:
    std::string _tbName;
    std::vector<std::vector<DataAttr>> _data;
};

class DeleteTable: public Action
{
public:
    DeleteTable(std::string tbName, std::vector<WhereClause> &whereClause): _tbName(tbName), _whereClause(whereClause) {}
    
    void execute(SystemManager &system) {
        system.del(_tbName, _whereClause);
    }
    void show() {
        printf("delete from table %s\n", _tbName.c_str());
    }
private:
    std::string _tbName;
    std::vector<WhereClause> _whereClause;
};

class UpdateTable: public Action
{
public:
    UpdateTable(std::string tbName, std::vector<WhereClause> &whereClause, std::vector<SetClause> &setClause): _tbName(tbName), _whereClause(whereClause), _setClause(setClause) {}
    
    void execute(SystemManager &system) {
        system.update(_tbName, _whereClause, _setClause);
    }
    void show() {
        printf("update table %s\n", _tbName.c_str());
    }
private:
    std::string _tbName;
    std::vector<WhereClause> _whereClause;
    std::vector<SetClause> _setClause;
};

class SelectTable: public Action
{
public:
    SelectTable(std::vector<std::string> &tbList, std::vector<Col> &selector, bool selectAll, std::vector<WhereClause> &whereClause):
        _tbList(tbList), _selector(selector), _selectAll(selectAll), _whereClause(whereClause) {}
    
    void execute(SystemManager &system) {
        system.select(_tbList, _selector, _selectAll, _whereClause);
    }
    
    void show() {
        printf("select table\n");
    }
private:
    std::vector<std::string> _tbList;
    std::vector<Col> _selector;
    bool _selectAll;
    std::vector<WhereClause> _whereClause;
};

#endif //DATABASE_TABLEACTION_H
