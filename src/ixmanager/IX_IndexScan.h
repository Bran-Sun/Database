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
    enum ScanType {
        FRONT, //从前往后
        BACK,   //从后往前
        MIDDLE  //直接搜
    };
    
public:
    IX_IndexScan(IX_IndexHandle &handle, AttrType attrType, int attrLength, ComOp comOp, const char *value):
        _handle(handle), _attrType(attrType), _attrLength(attrLength), _comOp(comOp), _value(value) {
        _init();
    }
    
    int getNextEntry(RID &rid);
    ~IX_IndexScan() {}
private:
    bool _satisfy(void *key);
    void _init();
    
private:
    IX_IndexHandle &_handle;
    AttrType _attrType;
    int  _attrLength;
    ComOp _comOp;
    const char *_value;
    std::shared_ptr<BpNode> _node;
    int _nodeIndex;
    ScanType _scanType;
};


#endif //DATABASE_IX_INDEXSCAN_H
