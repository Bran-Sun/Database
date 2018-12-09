//
// Created by 孙桢波 on 2018/11/23.
//

#ifndef DATABASE_IM_MANAGER_H
#define DATABASE_IM_MANAGER_H

#include "../bufmanager/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "../utils/pagedef.h"

#include "IX_IndexHandle.h"


class IX_Manager
{
public:
    IX_Manager(std::shared_ptr<FileManager> fm, std::shared_ptr<BufPageManager> bpm) {
        _fm = fm;
        _bpm = bpm;
    }
    
    int createIndex(const char *filename, int indexNo, AttrType attrtype, int attrLength);
    int destroyIndex(const char *filename, int indexNo);
    int openIndex(const char *filename, int indexNo, IX_IndexHandle &handle);
    int closeIndex(IX_IndexHandle &handle);
    
private:
    std::shared_ptr<FileManager> _fm;
    std::shared_ptr<BufPageManager> _bpm;
};


#endif //DATABASE_IM_MANAGER_H
