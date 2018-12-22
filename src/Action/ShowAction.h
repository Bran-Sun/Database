//
// Created by 孙桢波 on 2018-12-20.
//

#ifndef DATABASE_SHOWACTION_H
#define DATABASE_SHOWACTION_H

#include "Action.h"

class ShowDatabase : public Action
{
public:
    ShowDatabase() {}
    virtual void execute(SystemManager &system) {
    
    }
    
    virtual void show() {
        printf("show database!\n");
    }
};

class ShowDatabases : public Action
{
public:
    ShowDatabases() {}
    
    virtual void execute(SystemManager &system) {}
    
    virtual void show() {
        printf("show databases!\n");
    }
};


#endif //DATABASE_SHOWDATABASE_H
