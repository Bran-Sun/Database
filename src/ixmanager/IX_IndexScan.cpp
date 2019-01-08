//
// Created by 孙桢波 on 2018/11/26.
//

#include "IX_IndexScan.h"


int IX_IndexScan::getNextEntry(RID &rid)
{
    void *key = nullptr;
    int res = 0; //表示是否已经读完

    switch (_scanType) {
        case FRONT:
            res = _handle.getFrontNextItem(_node, _nodeIndex, key, rid);
            if (!_satisfy(key) || (res != 0)) {
                return -1;
            } else {
                return 0;
            }
        case BACK:
            res = _handle.getBackNextItem(_node, _nodeIndex, key, rid);
            if (!_satisfy(key) || (res != 0)) {
                return -1;
            } else {
                return 0;
            }
        case MIDDLE:
            res = _handle.getBackNextItem(_node, _nodeIndex, key, rid);
            if (res != 0 || !_satisfy(key))
            {
                return -1;
            } else {
                return 0;
            }
    }
}

bool IX_IndexScan::_satisfy(void *key)
{
    if (key == nullptr) return false;
    if (_value == nullptr) return true;
    
    return TypeCompWithComOp(key, _value, _attrType, _comOp, _attrLength);
}

void IX_IndexScan::_init() {
    _nodeIndex = -1;
    switch (_comOp) {
        case GE_OP:
        case GT_OP:
            _scanType = BACK;
            _node = _handle.getRightNode(_nodeIndex);
            break;
        case NE_OP:
        case LE_OP:
        case LT_OP:
            _scanType = FRONT;
            _node = _handle.getLeftNode(_nodeIndex);
            break;
        case EQ_OP:
            _scanType = MIDDLE;
            _node = _handle.getMiddleNode(_nodeIndex, _value);
            break;
        default:
            _scanType = FRONT;
            break;
    }
}