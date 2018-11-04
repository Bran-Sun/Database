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

class RM_Manager
{
public:
    RM_Manager(FileManager *fm, BufPageManager *bpm) {
        _fm = fm;
        _bpm = bpm;
    }
    
    int createFile(const char* filename, int recordSize);
    int destroyFile(const char* filename);
    int openFile(const char* filename, RM_FileHandle* handle);
    int closeFile(RM_FileHandle *handleFile);
    
    ~RM_Manager() { }
    
private:
    FileManager *_fm;
    BufPageManager *_bpm;
};


#endif //DATABASE_RM_MANAGER_H
