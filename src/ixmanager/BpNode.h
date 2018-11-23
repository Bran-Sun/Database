//
// Created by 孙桢波 on 2018/11/23.
//

#ifndef DATABASE_BPNODE_H
#define DATABASE_BPNODE_H

#include <vector>

class BpNode
{
public:
    BpNode() { _pageID = 0; _terminal = false; _leaf = false; }
    BpNode(int pageID, int index): _pageID(pageID), _index(index), _terminal(false), _leaf(false) {}
    
    bool isTerminal() const { return _terminal; }
    bool isLeaf() const { return _leaf; }
    
    void setTerminal(bool terminal) { _terminal = terminal; }
    void setLeaf(bool leaf) { _leaf = leaf; }
    
private:
    int _pageID, _index;
    bool _terminal, _leaf;
    std::vector<std::shared_ptr<void>> _keys;
    std::vector<int> _pagePointer;
};


#endif //DATABASE_BPNODE_H
