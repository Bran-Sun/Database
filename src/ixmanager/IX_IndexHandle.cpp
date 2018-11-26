//
// Created by 孙桢波 on 2018/11/23.
//

#include "IX_IndexHandle.h"

int IX_IndexHandle::insertEntry(void *pData, const RID &rid)
{
    std::shared_ptr<BpNode> node = _findKey(pData); //find most right node
    
    _insert(node, pData, rid);
    return 0;
}

int IX_IndexHandle::deleteEntry(void *pData, const RID &rid)
{
    std::shared_ptr<BpNode> node = _findKey(pData);
    
    int res = _delete(node, pData, rid);
    if (res < 0) {
        printf("IX_IndexHandle: delet entry dosen't exist!\n");
        return -1;
    }
    
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
    
    _bpm = bpm;
    _pageNumber = header->pageNumber;
    _recordNumber = header->recordNumber;
    _rootPos = header->rootPos;
    _maxKeyPerPage = header->maxKeyPerPage;
    _attrType = header->attrType;
    _attrlength = header->attrlength;
    
    std::cout << "pageNumber: " << _pageNumber << std::endl;
    std::cout << "recordNumber: " << _recordNumber << std::endl;
    std::cout << "rootPos: " << _rootPos << std::endl;
    std::cout << "maxKeyPerPage: " << _maxKeyPerPage << std::endl;
    
    //emptyList
    for (int i = 0; i < (PAGE_SIZE /4 - 6); i++) {
        if (header->emptyPage[i] > 0) {
            _emptyPageList.push_back(header->emptyPage[i]);
            _emptyPageSet.insert(header->emptyPage[i]);
        } else break;
    }
    
    //tree root;
    BufType bt;
    if (_rootPos == 0) { //init tree
        int pageID = 1, index;
        bt = _bpm->getPage(_fileID, pageID, index, IX_PAGE_SIZE);
        bt[0] = 1; //terminal
        bt[1] = 0; //prepage
        bt[2] = 0; //nextpage
        bt[3] = 0; //keyNums
        
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
        _rootPos = pageID;
        _pageNumber ++;
    } else {
        int index;
        bt = _bpm->allocPage(_fileID, _rootPos, index, true, IX_PAGE_SIZE);
    }
    _root = std::make_shared<BpNode>(BpNode(_rootPos, bt, _attrlength, true));
    
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

int IX_IndexHandle::_insert(std::shared_ptr<BpNode> node, void *pData, const RID &rid)
{
    int nodeIndex, result;
    result = node->findIndex(pData, nodeIndex, _attrType, _attrlength);
    if (result < 0) { //no such key
        _recordNumber++;
        _headerModify = true;
        _insertKey(node, pData, rid); //key label rid nodeIndex
    } else {
        bool find = false;
        std::shared_ptr<BpNode> cur = node;
        int nIndex = nodeIndex;
        while (!find) {
            if (nIndex < 0) {
                int pageID = node->getPrePage();
                if (pageID == 0) break;
                int index;
                BufType bt = _bpm->getPage(_fileID, pageID, index, IX_PAGE_SIZE);
                cur = std::make_shared<BpNode>(BpNode(nullptr, pageID, bt, _attrlength, false));
                nIndex = cur->getKeyNum() - 1;
            }
            if (cur->compKey(pData, _attrType, _attrlength, nIndex) == 0)
            {
                if ( (cur->getRID(nIndex)).equal(rid) )
                {
                    find = true;
                    break;
                } else {
                    nIndex--;
                }
            } else {
                break;
            }
        }
        
        if (find) {
            printf("IX_IndexHandle: entry has already been inserted!\n");
            return 1;
        }
        
        _recordNumber++;
        _headerModify = true;
        _insertKey(node, pData, rid);
    }
    
    return 0;
}

int IX_IndexHandle::_delete(std::shared_ptr<BpNode> node, void *pData, const RID &rid)
{
    int nodeIndex, result;
    result = node->findIndex(pData, nodeIndex, _attrType, _attrlength);
    
    if (result < 0) return -1;
    
    bool find = false;
    std::shared_ptr<BpNode> cur = node;
    int nIndex = nodeIndex;
    
    while (!find) {
        if (nIndex < 0) {
            int pageID = node->getPrePage();
            if (pageID == 0) break;
            int index;
            BufType bt = _bpm->getPage(_fileID, pageID, index, IX_PAGE_SIZE);
            cur = std::make_shared<BpNode>(BpNode(nullptr, pageID, bt, _attrlength, false));
            nIndex = cur->getKeyNum() - 1;
        }
        if (cur->compKey(pData, _attrType, _attrlength, nIndex) == 0)
        {
            if ( (cur->getRID(nIndex)).equal(rid) )
            {
                find = true;
                break;
            } else {
                nIndex--;
            }
        } else {
            break;
        }
    }
    
    if (!find) {
        return -1;
    }
    
    _headerModify = true;
    _recordNumber--;
    _deleteKey(cur);
    
    return 0;
}

std::shared_ptr<BpNode> IX_IndexHandle::_findKey(void *pData)
{
    std::shared_ptr<BpNode> cur = _root;
    int next;
    int index, result;
    while (!cur->isTerminal()) {
        result = cur->findIndex(pData, index, _attrType, _attrlength);
        next = cur->getChild(index);
        int pageIndex;
        BufType b = _bpm->getPage(_fileID, next, pageIndex, IX_PAGE_SIZE);
        std::shared_ptr<BpNode> nextNode = std::make_shared<BpNode>(BpNode(cur, next, b, _attrlength, false));
        
        cur = nextNode;
    }
    
    return cur;
}

int IX_IndexHandle::_insertKey(std::shared_ptr<BpNode> node, void *pData, const RID &rid)
{
    
    node->insertTerminalKV(pData, _attrlength, rid);
    
    if (node->getKeyNum() > _maxKeyPerPage) {
        int newPageID = _getEmptyPage();
        std::shared_ptr<BpNode> newNode = node->split(newPageID);
        int index;
        BufType bt = _bpm->getPage(_fileID, newPageID, index, IX_PAGE_SIZE);
        newNode->write(bt, _attrlength);
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
        
        int pageID = node->getPageID();
        bt = _bpm->getPage(_fileID, pageID, index, IX_PAGE_SIZE);
        node->write(bt, _attrlength);
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
        
        _insertInternalKey(node->getParent(), node, newNode);
    } else {
        int index;
        int pageID = node->getPageID();
        BufType bt = _bpm->getPage(_fileID, pageID, index, IX_PAGE_SIZE);
        node->write(bt, _attrlength);
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
    }
    
    return 0;
}

int IX_IndexHandle::_insertInternalKey(std::shared_ptr<BpNode> parent, std::shared_ptr<BpNode> lc,
                                       std::shared_ptr<BpNode> rc)
{
    if (parent == nullptr) { //root
        int pageID = _getEmptyPage();
        parent = std::make_shared<BpNode>(BpNode(true, false, pageID));
        _root = parent;
        _rootPos = pageID;
        //insert Key and lc, rc
        lc->setParent(parent);
        rc->setParent(parent);
        parent->initInsert(lc, rc, _attrlength);
        int index;
        BufType bt = _bpm->getPage(_fileID, pageID, index, IX_PAGE_SIZE);
        parent->write(bt, _attrlength);
        _bpm->markDirty(index);
        _modifyIndex.insert(index);
    } else {
        parent->insertInternalKey(rc, _attrlength);
        int pageID = parent->getPageID();
        int index;
        BufType bt = _bpm->getPage(_fileID, pageID, index, IX_PAGE_SIZE);
        if (parent->getKeyNum() > _maxKeyPerPage) {
            int newPageID = _getEmptyPage();
            std::shared_ptr<BpNode> newNode = parent->split(newPageID);
            parent->write(bt, _attrlength);
            _bpm->markDirty(index);
            _modifyIndex.insert(index);
    
            bt = _bpm->getPage(_fileID, newPageID, index, IX_PAGE_SIZE);
            newNode->write(bt, _attrlength);
            _bpm->markDirty(index);
            _modifyIndex.insert(index);
            
            _insertInternalKey(parent->getParent(), parent, newNode);
        } else {
            parent->write(bt, _attrlength);
            _bpm->markDirty(index);
            _modifyIndex.insert(index);
        }
    }
    return 0;
}

int IX_IndexHandle::_getEmptyPage()
{
    if (_emptyPageList.size() > 0) {
        int p = _emptyPageList.front();
        _emptyPageList.pop_front();
        _emptyPageSet.erase(p);
        return p;
    } else {
        _pageNumber++;
        return _pageNumber - 1;
    }
}

int IX_IndexHandle::_deleteKey(std::shared_ptr<BpNode> node)
{
    node->deleteKey();
    int pageID = node->getPageID();
    int index;
    BufType bt = _bpm->getPage(_fileID, pageID, index, IX_PAGE_SIZE);
    node->write(bt, _attrlength);
    _bpm->markDirty(index);
    _modifyIndex.insert(index);
    return 0;
}

std::shared_ptr<BpNode> IX_IndexHandle::getLeftNode() const
{
    std::shared_ptr<BpNode> cur = _root;
    while (!cur->isTerminal()) {
        int nextPageID = cur->getChild(-1);
        int pageIndex;
        BufType b = _bpm->getPage(_fileID, nextPageID, pageIndex, IX_PAGE_SIZE);
        std::shared_ptr<BpNode> nextNode = std::make_shared<BpNode>(BpNode(cur, nextPageID, b, _attrlength, false));
        cur = nextNode;
    }
    return cur;
}

int IX_IndexHandle::getNextItem(std::shared_ptr<BpNode> &node, int &nodeIndex, void *&key, RID &rid) const
{
    nodeIndex++;
    while (nodeIndex >= node->getKeyNum()) {
        int nextPageID = node->getNextPage();
        if (nextPageID == 0) return -1;
        int pageIndex;
        BufType b = _bpm->getPage(_fileID, nextPageID, pageIndex, IX_PAGE_SIZE);
        std::shared_ptr<BpNode> nextNode = std::make_shared<BpNode>(BpNode(nullptr, nextPageID, b, _attrlength, false));
        nodeIndex = 0;
        node = nextNode;
    }
    
    key = node->_keys[nodeIndex];
    rid = node->_rids[nodeIndex];
    return 0;
}

