//
// Created by 孙桢波 on 2018/11/3.
//

#ifndef DATABASE_RM_MANAGER_H
#define DATABASE_RM_MANAGER_H

#include "../fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include "RM_FileHandle.h"
#include "../utils/pagedef.h"
#include "PageHeaderFile.h"
#include "../smmanager/DataInfo.h"
#include "RMutils.h"

class RM_Manager
{
public:
    RM_Manager(std::shared_ptr<FileManager> fm, std::shared_ptr<BufPageManager> bpm) {
        _fm = fm;
        _bpm = bpm;
    }
    
    int createFile(const std::string &filename, std::vector<AttrInfo> attributes);
    int destroyFile(const std::string &filename);
    int openFile(const std::string &filename, RM_FileHandle &handle);
    int closeFile(RM_FileHandle &handleFile);
    
    ~RM_Manager() { }
    
private:
    std::shared_ptr<FileManager> _fm;
    std::shared_ptr<BufPageManager> _bpm;
};


#endif //DATABASE_RM_MANAGER_H
