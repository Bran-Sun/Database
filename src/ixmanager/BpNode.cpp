//
// Created by 孙桢波 on 2018/11/23.
//

#include <iostream>
#include "BpNode.h"
#include "IX_HeaderPage.h"

BpNode::BpNode(int pageID, BufType bt, int attrlength, bool root)
{
    _pageID = pageID;
    _load(bt, attrlength);

}

int BpNode::findIndex(void *pData, int &index, AttrType attrType, int attrlength)
{
    
    if (_keys.size() == 0)
    {
        index = -1;
        _hop = 0;
        return -1;
    }
    
    if (TypeComp(pData, _keys[0], attrType, attrlength) < 0) {
        index = -1;
        _hop = 0;
        return -1;
    }
    
    int begin = 0, end = _keys.size();
    while (begin < end - 1) {
        int mid = (begin + end) / 2;
        if (TypeComp(pData, _keys[mid], attrType, attrlength) < 0) {
            end = mid;
        } else {
            begin = mid;
        }
    }
    _hop = begin + 1;
    index = begin;
    
    if (TypeComp(pData, _keys[begin], attrType, attrlength) == 0) {
        return 0;
    }
    
    return -1;
}

BpNode::BpNode(std::shared_ptr<BpNode> parent, int pageID, BufType bt, int attrlength, bool root)
{
    _parent = parent;
    _root = root;
    _pageID = pageID;
    
    _load(bt, attrlength);
}

int BpNode::compKey(void *pData, AttrType type, int attrlength, int index)
{
    return TypeComp(pData, _keys[index], type, attrlength);
}

void BpNode::insertTerminalKV(void *pData, int attrlength, const RID &rid)
{
    char *data = new char[attrlength];
    memcpy(data, (char*)pData, (size_t) attrlength);
    _keys.insert(_keys.begin() + _hop, (void*)data);
    _rids.insert(_rids.begin() + _hop, rid);
}

std::shared_ptr<BpNode> BpNode::split(int pageID)
{
    std::shared_ptr<BpNode> node = std::make_shared<BpNode>(BpNode());
    node->_pageID = pageID;
    node->_nextPage = _nextPage;
    node->_prePage = _pageID;
    _nextPage = pageID;
    
    node->_terminal = _terminal;
    node->_root = _root;
    node->_parent = _parent;
    
    int size = _keys.size();
    int mid = size / 2;
    for (int i = mid; i < size; i++) {
        node->_keys.push_back(_keys[i]);
    }
    _keys.erase(_keys.begin() + mid, _keys.end());
    
    if (_terminal) {
        for (int i = mid; i < size; i++) {
            node->_rids.push_back(_rids[i]);
        }
        _rids.erase(_rids.begin() + mid, _rids.end());
    } else {
        node->_pageIndex.push_back(0);
        for (int i = mid + 1; i < size + 1; i++)
            node->_pageIndex.push_back(_pageIndex[i]);
        _pageIndex.erase(_pageIndex.begin() + mid + 1, _pageIndex.end());
    }
    return node;
}

void BpNode::_load(BufType bt, int attrlength)
{
    if (bt[0] == 0) _terminal = false;
    else _terminal = true;
    
    if (_terminal) {
        _prePage = bt[1];
        _nextPage = bt[2];
    }
    else {
        _prePage = 0;
        _nextPage = 0;
    }
    
    int keyNum = bt[3];
    
    if (!_terminal) {
        for ( int i = 0; i < keyNum + 1; i++ )
        {
            _pageIndex.push_back((int) bt[IX_NODE_H + i]);
        }
        
        for ( int i = 0; i < keyNum; i++ )
        {
            _pagePointer.push_back(nullptr);
        }
        
        char *p = (char *) bt + (keyNum + 1 + IX_NODE_H) * 4;
        for ( int i = 0; i < keyNum; i++ )
        {
            char *tem = new char[attrlength];
            memcpy(tem, p, attrlength);
            _keys.push_back((void *) tem);
            p = p + attrlength;
        }
    } else {
        for (int i = 0; i < keyNum; i++) {
            _rids.push_back(RID(bt[IX_NODE_H+i * 2], bt[IX_NODE_H + i * 2 + 1]));
        }
        
        char *p = (char *) bt + keyNum * 8 + IX_NODE_H * 4;
        for (int i = 0; i < keyNum; i++) {
            char *tem = new char[attrlength];
            memcpy(tem, p, attrlength);
            _keys.push_back((void*)tem);
            p = p + attrlength;
        }
    }
}

void BpNode::write(BufType bt, int attrlength)
{
    if (_terminal) bt[0] = 1;
    else bt[0] = 0;
    
    bt[1] = _prePage;
    bt[2] = _nextPage;
    bt[3] = _keys.size();
    
    if (_terminal)
    {
        for ( int i = 0; i < _keys.size(); i++)
        {
            int p , s;
            _rids[i].getRID(p, s);
            bt[IX_NODE_H + i * 2] = p;
            bt[IX_NODE_H + i * 2 + 1] = s;
        }
    
        char *p = (char *) bt + _keys.size() * 8 + IX_NODE_H * 4;
        for (int i = 0; i < _keys.size(); i++) {
            memcpy(p, (char*)_keys[i], attrlength);
            p = p + attrlength;
        }
    } else {
        for ( int i = 0; i < _keys.size() + 1; i++ )
        {
            bt[IX_NODE_H + i] = _pageIndex[i];
        }
        
        char *p = (char *) bt + (_keys.size() + 1 + IX_NODE_H) * 4;
        for ( int i = 0; i < _keys.size(); i++ )
        {
            memcpy(p, (char*)_keys[i], attrlength);
            p = p + attrlength;
        }
    }
}

void BpNode::initInsert(std::shared_ptr<BpNode> lc, std::shared_ptr<BpNode> rc, int attrlength)
{
    char *tem = new char[attrlength];
    memcpy(tem, (char*)rc->_keys[0], attrlength);
    _keys.push_back((void *) tem);
    _pageIndex.push_back(lc->_pageID);
    _pageIndex.push_back(rc->_pageID);
}

void BpNode::insertInternalKey(std::shared_ptr<BpNode> rc, int attrlength)
{
    char *tem = new char[attrlength];
    memcpy(tem, (char*)rc->_keys[0], attrlength);
    _keys.insert(_keys.begin() + _hop, (void*)tem);
    _pageIndex.insert(_pageIndex.begin() + _hop + 1, rc->getPageID());
}

void BpNode::deleteKey()
{
    if (_terminal) {
        _keys.erase(_keys.begin() + _hop - 1);
        _rids.erase(_rids.begin() + _hop - 1);
    }
}

BpNode::~BpNode()
{
    for (auto key : _keys) {
        delete[] (char*)key;
    }
}


