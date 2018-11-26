//
// Created by 孙桢波 on 2018/11/23.
//

#ifndef DATABASE_BPNODE_H
#define DATABASE_BPNODE_H

#include <vector>
#include "../utils/pagedef.h"
#include "../recordmanager/RID.h"

class BpNode
{
public:
    friend class IX_IndexHandle;
    BpNode() { _pageID = 0; _terminal = false; _leaf = false; _root = false; }
    BpNode(int pageID, BufType bt, int attrlength, bool root = false);
    BpNode(bool root, bool terminal, int pageID) : _root(root), _terminal(terminal), _pageID(pageID) { _prePage = 0; _nextPage = 0;}
    BpNode(std::shared_ptr<BpNode> parent, int pageID, BufType bt, int attrlength, bool root = false);
    
    bool isTerminal() const { return _terminal; }
    bool isLeaf() const { return _leaf; }
    bool isRoot() const { return _root; }
    int findIndex(void *pData, int &index, AttrType attrType, int attrlength);
    
    void setTerminal(bool terminal) { _terminal = terminal; }
    void setLeaf(bool leaf) { _leaf = leaf; }
    
    int getChild(int index) { return _pageIndex[index + 1]; }
    int getPrePage() const { return _prePage; }
    int getNextPage() const { return _nextPage; }
    int getKeyNum() const { return (int)_keys.size(); }
    int compKey(void *pData, AttrType type, int attrlength, int index);
    RID getRID(int index) const { return _rids[index]; }
    void insertTerminalKV(void* pData, int attrlength, const RID &rid);
    std::shared_ptr<BpNode> split(int pageID);
    void write(BufType bt, int attrlength);
    int getPageID() const { return _pageID; }
    std::shared_ptr<BpNode> getParent() { return _parent; }
    void setParent(std::shared_ptr<BpNode> parent) { _parent = parent; }
    void initInsert(std::shared_ptr<BpNode> lc, std::shared_ptr<BpNode>rc, int attrlength);
    void insertInternalKey(std::shared_ptr<BpNode> node, int attrlength);
    void deleteKey();
    
    ~BpNode();
    
private:
    void _load(BufType bt, int attrlength);
    
private:
    int _pageID, _nextPage, _prePage; //only is terminal then have nextPage
    bool _terminal, _leaf, _root;
    int _hop; //record formal search
    std::vector<void*> _keys;
    std::vector<int> _pageIndex;
    std::vector<std::shared_ptr<BpNode>> _pagePointer;
    std::vector<RID> _rids;
    std::shared_ptr<BpNode> _parent;
};


#endif //DATABASE_BPNODE_H
