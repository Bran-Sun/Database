//
// Created by 孙桢波 on 2018/11/23.
//

#include "IX_IndexHandle.h"

int IX_IndexHandle::insertEntry(void *pData, const RID &rid)
{
    std::shared_ptr<BpNode> node = _findKey(pData);
    int nodeIndex;
    int res = node->findKey(pData, nodeIndex);
    _insert(node, nodeIndex, pData, rid);
    return 0;
}

int IX_IndexHandle::deleteEntry(void *pData, const RID &rid)
{
    std::shared_ptr<BpNode> node = _findKey(pData);
    int nodeIndex;
    int res = node->findKey(pData, nodeIndex);
    if (res == -1) {
        printf("entry dosen't exist!\n");
        return -1;
    }
    
    _delete(node, nodeIndex, rid);
    
    return 0;
}

int IX_IndexHandle::forcePages()
{
    for (auto index : _modifyIndex) {
        _forcePage(index);
    }
    return 0;
}

void IX_IndexHandle::init(IX_HeaderPage *header, BufPageManager *bpm)
{
    _modifyIndex.clear();
    _emptyPageSet.clear();
    _emptyPageList.clear();
    
    _pageNumber = header->pageNumber;
    _recordNumber = header->recordNumber;
    _rootPos = header->rootPos;
    _maxKeyPerPage = header->maxKeyPerPage;
    _attrType = header->attrType;
    _attrlength = header->attrlength;
    
    //emptyList
    for (int i = 0; i < (PAGE_SIZE /4 - 6); i++) {
        if (header->emptyPage[i] > 0) {
            _emptyPageList.push_back(header->emptyPage[i]);
            _emptyPageSet.insert(header->emptyPage[i]);
        } else break;
    }
    
    //tree root;
    _root = new BpNode();
    
    _open = true;
    _headerModify = false;
    _bpm = bpm;
}

void IX_IndexHandle::setHeaderPage(IX_HeaderPage *header)
{
    header->pageNumber = _pageNumber;
    header->recordNumber = _recordNumber;
    header->rootPos = _rootPos;
    header->maxKeyPerPage = _maxKeyPerPage;
    header->attrType = _attrType;
    header->attrlength = _attrlength;
    
    //emptyList
    int count = 0;
    for (auto i : _emptyPageList) {
        header->emptyPage[count] = i;
        count++;
    }
    header->emptyPage[count] = 0;
}

void IX_IndexHandle::_forcePage(int index)
{
    int fileID, pageID;
    _bpm->getKey(index, fileID, pageID);
    if (fileID == _fileID) _bpm->writeBack(index, IX_PAGE_SIZE);
}

int IX_IndexHandle::_insert(std::shared_ptr<BpNode> node, int nodeIndex, void *pData, const RID &rid)
{
    node->insert
    return 0;
}

int IX_IndexHandle::_delete(std::shared_ptr<BpNode> node, int nodeIndex, const RID &rid)
{
    return 0;
}

std::shared_ptr<BpNode> IX_IndexHandle::_findKey(void *pData)
{
    std::shared_ptr<BpNode> cur = _root;
    while (!cur->isTerminal()) {
        cur = cur->findChild(pData);
    }
    return cur;
}

