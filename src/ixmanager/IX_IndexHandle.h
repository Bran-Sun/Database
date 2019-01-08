//
// Created by 孙桢波 on 2018/11/23.
//

#ifndef DATABASE_IX_INDEXHANDLE_H
#define DATABASE_IX_INDEXHANDLE_H

#include "../recordmanager/RID.h"
#include "../bufmanager/BufPageManager.h"
#include "IX_HeaderPage.h"
#include <list>
#include <set>
#include "BpNode.h"

class IX_IndexHandle
{
public:
    IX_IndexHandle() {
        _open = false;
        _headerModify = false;
        _fileID = -1;
        _bpm = nullptr;
    }
    ~IX_IndexHandle() {}
    
    void init(IX_HeaderPage *header, std::shared_ptr<BufPageManager> bpm);
    void setFileID(int fileID) { _fileID = fileID; }
    bool isOpen() const { return _open; }
    bool isHeaderModify() const { return _headerModify; }
    int getFileID() const { return _fileID; }
    void setHeaderPage(IX_HeaderPage *header);
    
    int insertEntry(const void *pData, const RID &rid);
    int deleteEntry(const void *pData, const RID &rid);
    int forcePages();
    AttrType getAttrType() const { return _attrType; }
    int getAttrlength() const { return _attrlength; }
    std::shared_ptr<BpNode> getLeftNode(int &index) const;
    std::shared_ptr<BpNode> getRightNode(int &index) const;
    std::shared_ptr<BpNode> getMiddleNode(int &index, const void *key) const;
    int getFrontNextItem(std::shared_ptr<BpNode> &node, int &nodeIndex, void *&key, RID &rid) const;
    int getBackNextItem(std::shared_ptr<BpNode> &node, int &nodeIndex, void *&key, RID &rid) const;
    
    bool notInIndex(const void *key) const;
    
private:
    void _forcePage(int index);
    std::shared_ptr<BpNode> _findKey(const void *pData) const;
    int _insert(std::shared_ptr<BpNode> node, const void *pData, const RID &rid);
    int _delete(std::shared_ptr<BpNode> node, const void *pData, const RID &rid);
    int _insertKey(std::shared_ptr<BpNode> node, const void *pData, const RID &rid);
    int _insertInternalKey(std::shared_ptr<BpNode> parent, std::shared_ptr<BpNode> lc, std::shared_ptr<BpNode> rc);
    int _getEmptyPage();
    int _deleteKey(std::shared_ptr<BpNode> node);

private:
    int _fileID;
    bool _open, _headerModify;
    std::shared_ptr<BufPageManager> _bpm;
    int _pageNumber, _recordNumber, _rootPos, _maxKeyPerPage;
    int _minKeyPerPage;
    AttrType _attrType;
    int _attrlength;
    std::list<int> _emptyPageList;
    std::set<int> _modifyIndex, _emptyPageSet;
    std::shared_ptr<BpNode> _root;
};


#endif //DATABASE_IX_INDEXHANDLE_H
