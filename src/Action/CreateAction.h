//
// Created by 孙桢波 on 2018-12-22.
//

#ifndef DATABASE_CREATEACTION_H
#define DATABASE_CREATEACTION_H

#include "Action.h"

class CreateDatabase : public Action
{
public:
    CreateDatabase(std::string dbName): _dbName(dbName) {}
    
    void execute(SystemManager &system) {
        system.createDb(_dbName);
    }
    
    void show() {
        printf("create database %s\n", _dbName.c_str());
    }
    
private:
    std::string _dbName;
};

class CreateTable : public Action
{
public:
    CreateTable(std::string tableName, std::vector<AttrInfo> attrInfo): _tableName(tableName), _attrInfo(attrInfo) {}
    void execute(SystemManager &system) {
        system.createTable(_tableName, _attrInfo);
    }
    void show() {
        printf("create table %s\n", _tableName.c_str());
    }
    
private:
    std::string _tableName;
    std::vector<AttrInfo> _attrInfo;
};

class CreateIndex : public Action
{
public:
    CreateIndex(std::string tableName, std::string indexName): _tableName(tableName), _indexName(indexName) {}
    void execute(SystemManager &system) {
        system.createIndex(_tableName, _indexName);
    }
    void show() {
        printf("create index %s in table %s\n", _indexName.c_str(), _tableName.c_str());
    }
private:
    std::string _tableName, _indexName;
};

#endif //DATABASE_CREATEACTION_H
