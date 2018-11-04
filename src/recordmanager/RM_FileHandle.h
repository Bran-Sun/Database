//
// Created by 孙桢波 on 2018/11/3.
//

#ifndef DATABASE_RM_FILEHANDLE_H
#define DATABASE_RM_FILEHANDLE_H

#include "RID.h"
#include "RM_Record.h"
#include "PageHeaderFile.h"
#include "../bufmanager/BufPageManager.h"
#include <list>
#include <vector>

class RM_FileHandle
{
private:
    int _fileID;
    bool _isOpen, _isHeaderModify;
    BufPageManager *_bpm;
    int _recordSize, _recordEachPage, _pageNum;
    std::list<int> _emptyPageList;
    std::vector<int> _modifyIndex;

private:
    void _forcePage(int index);
    
public:
    RM_FileHandle() {
        _bpm = nullptr;
        _fileID = -1;
    }
    
    ~RM_FileHandle() {
        _emptyPageList.clear();
    }
    
    int getRecord(const RID &rid, RM_Record &record) const;
    int insertRecord(const char* data, RID &rid);
    int deleteRecord(RID &rid);
    int updateRecord(const RM_Record &record);
    int forcePages();
    void init(PageHeaderFile* header, BufPageManager* bpm);
    
    void setFileID(int fileID) {
        _fileID = fileID;
    }
    
    int getFileID() const {
        return _fileID;
    }
    
    bool isOpen() const {
        return _isOpen;
    }
    
    bool isHeaderModify() const {
        return _isHeaderModify;
    }
    
    void setClose() {
        _isOpen = false;
    }
    
    void setHeaderPage(PageHeaderFile *header);
};


#endif //DATABASE_RM_FILEHANDLE_H
