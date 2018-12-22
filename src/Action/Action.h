//
// Created by 孙桢波 on 2018-12-20.
//

#ifndef DATABASE_ACTION_H
#define DATABASE_ACTION_H

#include "../qlmanager/SystemManager.h"

class Action
{
public:
    virtual void execute(SystemManager &system) = 0;
    virtual void show() = 0;
private:

};


#endif //DATABASE_ACTION_H
