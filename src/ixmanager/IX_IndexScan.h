//
// Created by 孙桢波 on 2018/11/26.
//

#ifndef DATABASE_IX_INDEXSCAN_H
#define DATABASE_IX_INDEXSCAN_H

#include "IX_IndexHandle.h"
#include "../utils/pagedef.h"

class IX_IndexScan
{
public:
    IX_IndexScan() { _value = nullptr; _handle = nullptr; _node = nullptr; }
    ~IX_IndexScan() {}
    
    int openScan(const IX_IndexHandle *handle, ComOp comOp, void *value);
    int getNextEntry(RID &rid);
    int closeScan();
    
private:
    bool _satisfy(void *key);
    
private:
    const IX_IndexHandle *_handle;
    AttrType _attrType;
    int  _attrlength;
    ComOp _comOp;
    void *_value;
    std::shared_ptr<BpNode> _node;
    int _nodeIndex;
};


#endif //DATABASE_IX_INDEXSCAN_H
