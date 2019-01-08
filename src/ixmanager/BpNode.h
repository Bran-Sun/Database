#include <utility>

//
// Created by 孙桢波 on 2018/11/23.
//

#ifndef DATABASE_BPNODE_H
#define DATABASE_BPNODE_H

#include <vector>
#include "../utils/pagedef.h"
#include "../recordmanager/RID.h"

#define IX_NODE_H 4

struct Key {
    std::string data;
    Key(const char* p, int length) {
        data = std::string(p, length);
    }
    
    Key(const void* p, int length) {
        data = std::string((char*)p, length);
    }
};

class BpNode
{
public:
    friend class IX_IndexHandle;
    BpNode() { _pageID = 0; _terminal = false; _leaf = false; _root = false; }
    BpNode(int pageID, BufType bt, int attrlength, bool root = false);
    BpNode(bool root, bool terminal, int pageID, BufType bt) : _root(root), _terminal(terminal), _pageID(pageID), _buf(bt) { _prePage = 0; _nextPage = 0; _keyNum = 0;}
    BpNode(std::shared_ptr<BpNode> parent, int pageID, BufType bt, int attrlength, bool root = false);
    
    bool isTerminal() const { return _terminal; }
    bool isLeaf() const { return _leaf; }
    bool isRoot() const { return _root; }
    int findIndex(const void *pData, int &index, AttrType attrType, int attrlength);
    
    void setTerminal(bool terminal) { _terminal = terminal; }
    void setLeaf(bool leaf) { _leaf = leaf; }
    
    int getChild(int index) { return _buf[IX_NODE_H + index + 1]; }
    int getPrePage() const { return _prePage; }
    int getNextPage() const { return _nextPage; }
    int getKeyNum() const { return _keyNum; }
    int compKey(const void *pData, AttrType type, int attrlength, int index);
    RID getRID(int index) const { return RID(_buf[IX_NODE_H + index * 2], _buf[IX_NODE_H + index * 2 + 1]); }
    void insertTerminalKV(const void* pData, int attrlength, const RID &rid);
    std::shared_ptr<BpNode> split(int pageID);
    void write(BufType bt, int attrlength);
    int getPageID() const { return _pageID; }
    std::shared_ptr<BpNode> getParent() { return _parent; }
    void setParent(std::shared_ptr<BpNode> parent) { _parent = std::move(parent); }
    void initInsert(std::shared_ptr<BpNode> lc, std::shared_ptr<BpNode>rc, int attrlength);
    void insertInternalKey(std::shared_ptr<BpNode> node, int attrlength);
    void deleteKey(BufType bt, int attrlength);
    
    char* getLightKey(int nodeIndex, int attrLength) { //maybe not safe for other use
        if (!_terminal) {
            return (char*)(_buf + IX_NODE_H + _keyNum + 1) + attrLength * nodeIndex;
        } else {
            return (char*)(_buf + IX_NODE_H + _keyNum * 2) + attrLength * nodeIndex;
        }
    }
    
    RID getLightRID(int nodeIndex) {
        if (_terminal) {
            return {(int)_buf[IX_NODE_H + nodeIndex * 2], (int)_buf[IX_NODE_H + nodeIndex * 2 + 1]};
        } else {
            return {0, 0};
        }
    }
    
    
    ~BpNode();

private:
    void _load(BufType bt, int attrlength);
    void _lightLoad(BufType bt, int attrlength);

private:
    int _pageID, _nextPage, _prePage, _keyNum; //only is terminal then have nextPage
    bool _terminal, _leaf, _root;
    int _hop; //record formal search
    BufType _buf;
    std::vector<Key> _keys;
    std::vector<int> _pageIndex;
    std::vector<std::shared_ptr<BpNode>> _pagePointer;
    std::vector<RID> _rids;
    std::shared_ptr<BpNode> _parent;
};


#endif //DATABASE_BPNODE_H