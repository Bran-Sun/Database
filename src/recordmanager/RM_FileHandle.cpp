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
    int flag = b[p] >> (slotID - (p << 5));
    if (!(flag & 0x1)) return -1;   //记录不存在
    
    charp start = (charp)b;
    start = start + RECORD_MAP + slotID * _recordSize;
    
    record.setData(start, _recordSize, &rid);
    
    return 0;
}

int RM_FileHandle::insertRecord(const char *data, RID &rid)
{
    if (!_isOpen) return -1;
    
    if (_emptyPageList.size() == 0) {
        int index;
        BufType b = _bpm->allocPage(_fileID, _pageNum, index, false);
        for (int i = 0; i < RECORD_MAP / 4; i++) {
            b[i] = 0;
        }
        _emptyPageList.push_back(_pageNum);
        _emptyPageSet.insert(_pageNum);
        _pageNum++;
        _isHeaderModify = true;
        
        b[0] = 1;
        rid.setRID(_pageNum - 1, 0);
        charp start = (charp)b + RECORD_MAP;
        memcpy(start, data, (size_t) _recordSize);
        
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
    } else {
        int pageID = _emptyPageList.front();
        int index;
        BufType b = _bpm->getPage(_fileID, pageID, index);
        int slot = _getEmptySlot(b);
        _setEmptySlot(b, slot);
        
        rid.setRID(pageID, slot);
        charp start = (charp)b;
        start = start + RECORD_MAP + slot * _recordSize;
        memcpy(start, data, _recordSize);
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
        
        int slotCheck = _getEmptySlot(b);
        if (slotCheck == _recordSize) {
            _emptyPageList.pop_front();
            _emptyPageSet.erase(pageID);
        }
        _isHeaderModify = true;
        
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
    }
    return 0;
}

int RM_FileHandle::deleteRecord(RID &rid)
{
    if (!_isOpen) return -1;
    int pageID, slotID, index;
    rid.getRID(pageID, slotID);
    if (pageID == 0 || pageID >= _pageNum || slotID >= _recordEachPage) return -1;  //页号和槽号不满足要求
    
    BufType b = _bpm->getPage(_fileID, pageID, index);
    int p = slotID >> 5;
    int flag = b[p] >> (slotID - (p << 5));
    if (!(flag & 0x1)) return 0;   //记录不存在
    else {
        //判断原先表是否为满
        bool originFull = _checkOriginEmpty(pageID);
        b[p] = b[p] ^ (1 << (slotID - (p << 5)));
        if (originFull) {
            _emptyPageList.push_front(pageID);
            _emptyPageSet.insert(pageID);
            _isHeaderModify = true;
        }
        
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
    }
    return 0;
}

int RM_FileHandle::updateRecord(const RM_Record &record)
{
    if (!_isOpen) return -1;
    int pageID, slotID, index;
    record.getRIDContent(pageID, slotID);
    if (pageID == 0 || pageID >= _pageNum || slotID >= _recordEachPage) return -1;  //页号和槽号不满足要求
    
    BufType b = _bpm->getPage(_fileID, pageID, index);
    int p = slotID >> 5;
    int flag = b[p] >> (slotID - (p << 5));
    if (!(flag & 0x1)) return 0;   //记录不存在
    
    charp start = (charp)b + RECORD_MAP + slotID * _recordSize;
    charp newData;
    record.getData(newData);
    memcpy(start, newData, _recordSize);
    
    _bpm->markDirty(index);
    _modifyIndex.insert(index);
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
    _emptyPageSet.clear();
    
    _recordSize = header->recordSize;
    _recordEachPage = header->recordEachPage;
    _pageNum = header->pageNumber;
    for (int i = 0; i < (PAGE_SIZE / 4 - 3); i++) {
        if (header->emptyPageList[i] > 0) {
            _emptyPageList.push_back(header->emptyPageList[i]);
            _emptyPageSet.insert(header->emptyPageList[i]);
        }
        else break;
    }
    _isOpen = true;
    _isHeaderModify = false;
    _bpm = bpm;
}

int RM_FileHandle::_getEmptySlot(BufType b)
{
    int count = 0;
    int i = 0, j = 0;
    while (count < _recordSize) {
        if (!((b[i] >> j) & 0x1)) break;
        
        j++;
        if (j == 32) {
            j = 0;
            i++;
        }
        count++;
    }
    return count;
}

bool RM_FileHandle::_checkOriginEmpty(int pageID)
{
    return (_emptyPageSet.find(pageID) != _emptyPageSet.end());
}

void RM_FileHandle::setHeaderPage(PageHeaderFile *header)
{
    header->recordSize = _recordSize;
    header->recordEachPage = _recordEachPage;
    header->pageNumber = _pageNum;
    
    int count = 0;
    for (auto i : _emptyPageList) {
        header->emptyPageList[count] = i;
        count++;
    }
    header->emptyPageList[count] = 0;
}

void RM_FileHandle::_setEmptySlot(BufType b, int slot)
{
    int p = slot >> 5;
    int left = slot - (p << 5);
    b[p] = b[p] | (1 << left);
}

