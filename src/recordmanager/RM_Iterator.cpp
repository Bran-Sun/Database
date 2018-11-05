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
    switch (_attrType) {
        case INT:
            return _compInt(compData);
        case BOOL:
            return _compBool(compData);
        case FLOAT:
            return _compFloat(compData);
        case STRING:
            return _compString(compData);
        case CHAR:
            return _compChar(compData);
    }
    return false;
}

bool RM_Iterator::_compInt(char *data)
{
    int value = *(int*)_value;
    int compData = ((int*)data)[0];
    switch (_comOp) {
        case EQ_OP:
            return value == compData;
        case LT_OP:
            return value < compData;
        case GT_OP:
            return value > compData;
        case LE_OP:
            return value <= compData;
        case GE_OP:
            return value >= compData;
        case NE_OP:
            return value != compData;
        case NO_OP:
            return true;
        
    }
}

bool RM_Iterator::_compBool(char *data)
{
    bool value = *(bool*)_value;
    bool compData = ((bool*)data)[0];
    switch(_comOp) {
        case EQ_OP:
            return value == compData;
        case NE_OP:
            return value != compData;
        default:
            return false;
    }
}

bool RM_Iterator::_compFloat(char *data)
{
    float value = *(float*)_value;
    float compData = ((float*)data)[0];
    switch (_comOp) {
        case EQ_OP:
            return value == compData;
        case LT_OP:
            return value < compData;
        case GT_OP:
            return value > compData;
        case LE_OP:
            return value <= compData;
        case GE_OP:
            return value >= compData;
        case NE_OP:
            return value != compData;
        case NO_OP:
            return true;
    }
    return false;
}

bool RM_Iterator::_compString(char *data)
{
    std::string value((char*)_value, _attrLength);
    std::string compData(data, _attrLength);
    
    switch (_comOp) {
        case EQ_OP:
            return value == compData;
        case LT_OP:
            return value < compData;
        case GT_OP:
            return value > compData;
        case LE_OP:
            return value <= compData;
        case GE_OP:
            return value >= compData;
        case NE_OP:
            return value != compData;
        case NO_OP:
            return true;
    }
    return false;
}

bool RM_Iterator::_compChar(char *data)
{
    char value = *(char*)_value;
    char compData = data[0];
    switch (_comOp) {
        case EQ_OP:
            return value == compData;
        case LT_OP:
            return value < compData;
        case GT_OP:
            return value > compData;
        case LE_OP:
            return value <= compData;
        case GE_OP:
            return value >= compData;
        case NE_OP:
            return value != compData;
        case NO_OP:
            return true;
    }
}
