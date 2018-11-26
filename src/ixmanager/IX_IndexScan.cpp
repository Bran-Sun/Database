//
// Created by 孙桢波 on 2018/11/26.
//

#include "IX_IndexScan.h"

int IX_IndexScan::openScan(const IX_IndexHandle *handle, ComOp comOp, void *value)
{
    _handle = handle;
    _comOp = comOp;
    _value = value;
    
    _node = handle->getLeftNode();
    _nodeIndex = -1;
    _attrType = handle->getAttrType();
    _attrlength = handle->getAttrlength();
    
    return 0;
}

int IX_IndexScan::getNextEntry(RID &rid)
{
    void *key = nullptr;
    int res = 0; //表示是否已经读完
    while (!_satisfy(key) && res == 0)
    {
        res = _handle->getNextItem(_node, _nodeIndex, key, rid);
    }
    return res;
}

int IX_IndexScan::closeScan()
{
    return 0;
}

bool IX_IndexScan::_satisfy(void *key)
{
    if (key == nullptr) return false;
    if (_value == nullptr) return true;
    
    return TypeCompWithComOp(key, _value, _attrType, _comOp, _attrlength);
    return false;
}
