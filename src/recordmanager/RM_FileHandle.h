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
#include <set>
#include "../smmanager/DataInfo.h"

#define RM_HEADER_LEN 1
#define EMPTYNEXT 0

class RM_FileHandle
{
public:
    RM_FileHandle() {
        _bpm = nullptr;
        _isOpen = false;
        _isHeaderModify = false;
        _fileID = -1;
    }
    
    ~RM_FileHandle() { }
    
    std::vector<AttrInfo> getAttrInfo() { return _attributions; }
    int getRecord(const RID &rid, RM_Record &record) const;
    int getNextRecord(RID &ridIn, RM_Record &record, int offset = 1) const;
    int insertRecord(const char* data, RID &rid);
    int deleteRecord(RID &rid);
    int updateRecord(const RM_Record &record);
    int forcePages();
    void init(PageHeaderFile* header, std::shared_ptr<BufPageManager> bpm);
    
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
    
    void addIndex(int indexNo) { _attributions[indexNo].isIndex = true; _isHeaderModify = true; }
    void dropIndex(int indexNo) { _attributions[indexNo].isIndex = false; _isHeaderModify = true; }
    
private:
    void _forcePage(int index);
    int _getEmptySlot(BufType b);
    void _setEmptySlot(BufType b, int slot);

private:
    int _fileID;
    bool _isOpen, _isHeaderModify;
    std::shared_ptr<BufPageManager> _bpm;
    int _recordSize, _recordEachPage, _pageNum;
    std::set<int> _modifyIndex;
    std::vector<AttrInfo> _attributions;
    int _emptyPageHead;
};


#endif //DATABASE_RM_FILEHANDLE_H
