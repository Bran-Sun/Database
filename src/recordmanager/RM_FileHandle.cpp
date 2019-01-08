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
    int flag = b[RM_HEADER_LEN + p] >> (slotID - (p << 5));
    if (!(flag & 0x1)) return -1;   //记录不存在
    
    charp start = (charp)b;
    start = start + RM_HEADER_LEN * 4  + RECORD_MAP + slotID * _recordSize;
    
    record.setData(start, _recordSize, &rid);
    
    return 0;
}

int RM_FileHandle::insertRecord(const char *data, RID &rid)
{
    if (!_isOpen) return -1;
    if (_emptyPageHead == 0) {
        int index;
        BufType b = _bpm->allocPage(_fileID, _pageNum, index, false);
        _emptyPageHead = _pageNum;
        _pageNum++;
        _isHeaderModify = true;
        
        b[0] = 0;
        b[1] = 1;
        for (int i = 2; i < 9; i++)
            b[i] = 0;
        rid.setRID(_pageNum - 1, 0);
        char *start = (char *)b + RM_HEADER_LEN * 4 + RECORD_MAP;
        memcpy(start, data, (size_t)_recordSize);
        
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
    } else {
        int pageID = _emptyPageHead;
        int index;
        BufType b = _bpm->getPage(_fileID, pageID, index);
        int slot = _getEmptySlot(b + 1);
        _setEmptySlot(b + 1, slot);
        
        rid.setRID(pageID, slot);
        char *start = (char*)b;
        start = start + RM_HEADER_LEN * 4 + RECORD_MAP + slot * _recordSize;
        memcpy(start, data, (size_t)_recordSize);
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
        
        int slotCheck = _getEmptySlot(b + 1, slot);
        if (slotCheck == _recordEachPage) {
            if (b[0] != 0) {
                _emptyPageHead = b[0];
                b[0] = 0;
            } else {
                _emptyPageHead = 0;
            }
            _isHeaderModify = true;
        }
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
    int flag = b[p + RM_HEADER_LEN] >> (slotID - (p << 5));
    if (!(flag & 0x1)) {
        printf("rm_handle: record not exist!\n");
        return 1;   //记录不存在
    }
    else {
        //判断原先表是否为满
        b[p + RM_HEADER_LEN] = b[p + RM_HEADER_LEN] ^ (1 << (slotID - (p << 5)));
        if (b[EMPTYNEXT] == 0) {
            b[EMPTYNEXT] = _emptyPageHead;
            _emptyPageHead = pageID;
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
    int flag = b[RM_HEADER_LEN + p] >> (slotID - (p << 5));
    if (!(flag & 0x1)) {
        printf("rm_handle: record not exist!\n");
        return 0;   //记录不存在
    }
    std::string tem = record._data.substr(0, 4);
    int refer = *(int*)(tem.c_str());
    charp start = (charp)b + RM_HEADER_LEN * 4 + RECORD_MAP + slotID * _recordSize;
    memcpy(start, record._data.c_str(), (size_t)_recordSize);
    
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

void RM_FileHandle::init(PageHeaderFile *header, std::shared_ptr<BufPageManager> bpm)
{
    _modifyIndex.clear();
    
    _recordSize = header->recordSize;
    _recordEachPage = header->recordEachPage;
    _pageNum = header->pageNumber;
    _emptyPageHead = header->emptyPageHead;
    
    int attrNum = header->attrNumber;
    for (int i = 0; i < attrNum; i++) {
        _attributions.emplace_back(header->attributions[i]);
    }
    
    _isOpen = true;
    _isHeaderModify = false;
    _bpm = bpm;
}

int RM_FileHandle::_getEmptySlot(BufType b, int offset)
{
    int count = offset;
    int i = offset / 32;
    int j = offset - i * 32;
    while (count < _recordEachPage) {
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

void RM_FileHandle::setHeaderPage(PageHeaderFile *header)
{
    header->recordSize = _recordSize;
    header->recordEachPage = _recordEachPage;
    header->pageNumber = _pageNum;
    header->emptyPageHead = _emptyPageHead;
    
    for (int i = 0; i < _attributions.size(); i++) {
        header->attributions[i].isIndex = _attributions[i].isIndex;
    }
}

void RM_FileHandle::_setEmptySlot(BufType b, int slot)
{
    int p = slot >> 5;
    int left = slot - (p << 5);
    b[p] = b[p] | (1 << left);
}

int RM_FileHandle::getNextRecord(RID &ridIn, RM_Record &record, int offset) const
{
    if (!_isOpen) {
        printf("rmHandle not open!\n");
        return -1;
    }
    
    int pageID, slotID, index;
    ridIn.getRID(pageID, slotID);
    
    slotID = slotID + offset;
    if (pageID == 0 || pageID >= _pageNum || slotID > _recordEachPage) {
        printf("pageID: %d and slotID: %d not satisfied!\n", pageID, slotID);
        return -1;  //页号和槽号不满足要求
    }
    
    bool find = false;
    BufType b;
    int p = 0;
    int q = 0;
    int count = 0;
    
    if (slotID < _recordEachPage)
    {
        b = _bpm->getPage(_fileID, pageID, index);
    
        p = slotID >> 5;
        q = slotID - (p << 5);
        count = slotID;
        while ( count < _recordEachPage )
        {
            if ((b[p + RM_HEADER_LEN] >> q) & 0x1 )
            {
                find = true;
                break;
            } else
            {
                q++;
                count++;
                if ( q == 32 )
                {
                    q = 0;
                    p++;
                }
            }
        }
    }
    
    if (!find) {
        pageID++;
        while (pageID < _pageNum) {
            b = _bpm->getPage(_fileID, pageID, index);

            p = 0;
            q = 0;
            count = 0;
            while (count < _recordEachPage) {
                if ((b[RM_HEADER_LEN + p] >> q) & 0x1) {
                    find = true;
                    break;
                } else {
                    q++;
                    count++;
                    if (q == 32) {
                        q = 0;
                        p++;
                    }
                }
            }
            if (find) break;
            pageID++;
        }
    }
    
    if (!find) return -1;
    charp start = (charp)b + RM_HEADER_LEN * 4 + count * _recordSize + RECORD_MAP;
    record.setData(start, _recordSize, pageID, count);
    ridIn.setRID(pageID, count);
    return 0;
}



