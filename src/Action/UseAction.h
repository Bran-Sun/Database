//
// Created by 孙桢波 on 2018-12-22.
//

#ifndef DATABASE_USEACTION_H
#define DATABASE_USEACTION_H

#include "Action.h"

class UseDatabase: public Action
{
public:
    UseDatabase(std::string dbName): _dbName(dbName) {}
    void execute(SystemManager &system) {}
    
    void show() {
        printf("use database %s\n", _dbName.c_str());
    }
private:
    std::string _dbName;
};

#endif //DATABASE_USEACTION_H
