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
        system.showDb();
    }
    
    virtual void show() {
        printf("show database!\n");
    }
};

class ShowTables : public Action
{
public:
    ShowTables() {}
    
    virtual void execute(SystemManager &system) {
        system.showTable();
    }
    
    virtual void show() {
        printf("show tables!\n");
    }
};


#endif //DATABASE_SHOWDATABASE_H
