//
// Created by 孙桢波 on 2018/11/5.
//

#ifndef DATABASE_RM_ITERATOR_H
#define DATABASE_RM_ITERATOR_H

#include "../utils/pagedef.h"
#include "RM_Record.h"
#include "RM_FileHandle.h"

class RM_Iterator
{
private:
    RM_FileHandle *_handle;
    RID _rid;
    AttrType _attrType;
    int _attrLength;
    int _attrOffset;
    ComOp _comOp;
    void *_value;

private:
    bool _satisfy(const RM_Record &record);
    
    bool _compInt(char *data);
    bool _compBool(char *data);
    bool _compFloat(char *data);
    bool _compString(char *data);
    bool _compChar(char *data);
    
public:
    RM_Iterator() {
        _handle = nullptr;
        _value = nullptr;
        _rid.setRID(0, 0);
    }
    
    void initIterator(RM_FileHandle *handle, AttrType type, int attrLength, int attrOffset, ComOp comOp, void *value) {
        _handle = handle;
        _attrType = type;
        _attrLength = attrLength;
        _attrOffset = attrOffset;
        _comOp = comOp;
        _value = value;
        _rid.setRID(0, -1);
    }
    
    int getNextRecord(RM_Record &record);
    ~RM_Iterator() {}
};


#endif //DATABASE_RM_ITERATOR_H