//
// Created by 孙桢波 on 2018/11/5.
//

#include "RM_Iterator.h"

int RM_Iterator::getNextRecord(RM_Record &record) {
    int result = _handle->getNextRecord(_rid, record);
    
    while ((result != -1) && !_satisfy(record)) {
        result = _handle->getNextRecord(_rid, record);
    }
    
    return result;
}

bool RM_Iterator::_satisfy(const RM_Record &record)
{
    if (_value == nullptr) return true;
    char *data = nullptr;
    record.getData(data);
    
    char *compData = data + _attrOffset;
    return false;
}
