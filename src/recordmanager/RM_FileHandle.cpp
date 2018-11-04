//
// Created by 孙桢波 on 2018/11/3.
//

#include "RM_FileHandle.h"

int RM_FileHandle::getRecord(const RID &rid, RM_Record &record) const
{
    if (!_isOpen) return -1;    //文件未打开
    int pageID, slotID, index;
    rid.getRID(pageID, slotID);
    
    if (pageID == 0 || pageID >= _pageNum || slotID >= _recordEachPage) return -1;  //页号和槽号不满足要求
    BufType b = _bpm->getPage(_fileID, pageID, index);
    
    int p = slotID >> 5;
    int flag = b[p] >> (slotID - p << 5);
    if (!(flag & 0x1)) return -1;   //记录不存在
    
    
    
    
    return 0;
}

int RM_FileHandle::insertRecord(const char *data, RID &rid)
{
    return 0;
}

int RM_FileHandle::deleteRecord(RID &rid)
{
    return 0;
}

int RM_FileHandle::updateRecord(const RM_Record &record)
{
    return 0;
}

int RM_FileHandle::forcePages()
{
    for (auto index : _modifyIndex) {
        _forcePage(index);
    }
    return 0;
}

void RM_FileHandle::_forcePage(int index) {
    int fileID, pageID;
    _bpm->getKey(index, fileID, pageID);
    if (fileID == _fileID) _bpm->writeBack(index);
}

void RM_FileHandle::init(PageHeaderFile *header, BufPageManager *bpm)
{
    _modifyIndex.clear();
    _emptyPageList.clear();
    
    _recordSize = header->recordSize;
    _recordEachPage = header->recordEachPage;
    _pageNum = header->pageNumber;
    for (int i = 0; i < (PAGE_SIZE / 4 - 3); i++) {
        if (header->emptyPageList[i] > 0) _emptyPageList.push_back(header->emptyPageList[i]);
        else break;
    }
    _isOpen = true;
    _isHeaderModify = false;
    _bpm = bpm;
}
