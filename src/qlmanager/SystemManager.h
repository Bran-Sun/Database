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

private:
    SM_Manager sm_manager;
    DatabaseHandle currentDb;
};


#endif //DATABASE_SYSTEMMANAGER_H
