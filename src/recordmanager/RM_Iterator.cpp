//
// Created by 孙桢波 on 2018/11/5.
//

#include "RM_Iterator.h"

//结束时返回-1
int RM_Iterator::getNextRecord(RM_Record &record) {
    int result = _handle.getNextRecord(_rid, record);
    while ((result != -1) && !_satisfy(record)) {
        result = _handle.getNextRecord(_rid, record, 1);
    }
    
    return result;
}

bool RM_Iterator::_satisfy(const RM_Record &record)
{
    if (_value == nullptr) return true;
    std::string data;
    data = record.getData();
    
    const char *compData;
    compData = data.c_str() + _attrOffset;
    return TypeCompWithComOp((void*)compData, _value, _attrType, _comOp, _attrLength);
}
