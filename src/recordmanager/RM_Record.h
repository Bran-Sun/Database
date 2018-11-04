//
// Created by 孙桢波 on 2018/11/3.
//

#ifndef DATABASE_RM_RECORD_H
#define DATABASE_RM_RECORD_H

#include "RID.h"
#include <cstdlib>
#include <cstring>

class RM_Record
{
private:
    char *_data;
    const RID *_rid;
    int _recordSize;
    
public:
    RM_Record() {
        _data = nullptr;
        _rid = nullptr;
        _recordSize = 0;
    }
    
    void getData(char *&data) const{
        data = _data;
    }
    
    void getRid(RID &rid) const {
        rid = *_rid;
    }
    
    void setData(const char *data, int recordSize, const RID *rid) {
        if (_data != nullptr) {
            free(_data);
        }
        
        _data = (char*)malloc(sizeof(char) * recordSize);
        memcpy(_data, data, (size_t)recordSize);
        _recordSize = recordSize;
        _rid = rid;
    }
    
    void getRIDContent(int &pageID, int &slotID) const {
        _rid->getRID(pageID, slotID);
    }
};


#endif //DATABASE_RM_RECORD_H
