//
// Created by 孙桢波 on 2018-12-22.
//

#ifndef DATABASE_DROPACTION_H
#define DATABASE_DROPACTION_H

#include "Action.h"

class DropDatabase : public Action
{
public:
    DropDatabase(std::string dbName): _dbName(dbName) {}
    void execute(SystemManager &system) {}
    void show() {
        printf("drop database %s\n", _dbName.c_str());
    }
private:
    std::string _dbName;
};

class DropTable : public Action
{
public:
    DropTable(std::string tableName): _tableName(tableName) {}
    void execute(SystemManager &system) {}
    void show() {
        printf("drop table %s\n", _tableName.c_str());
    }
private:
    std::string _tableName;
};

class DropIndex : public Action
{
public:
    DropIndex(std::string tableName, std::string indexName): _tableName(tableName), _indexName(indexName) {}
    void execute(SystemManager &system) {}
    void show() {
        printf("drop index %s in table %s\n", _indexName.c_str(), _tableName.c_str());
    }
private:
    std::string _tableName, _indexName;
};

#endif //DATABASE_DROPACTION_H
