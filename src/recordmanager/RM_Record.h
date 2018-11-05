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
    int _recordSize;
    int _pageID, _slotID;
    
public:
    RM_Record() {
        _data = nullptr;
        _recordSize = 0;
    }
    
    void getData(char *&data) const{
        data = _data;
    }
    
    void getRid(RID &rid) const {
        rid.setRID(_pageID, _slotID);
    }
    
    void setData(const char *data, int recordSize, const RID *rid) {
        if (_data != nullptr) {
            free(_data);
        }
        
        _data = (char*)malloc(sizeof(char) * recordSize);
        memcpy(_data, data, (size_t)recordSize);
        _recordSize = recordSize;
        rid->getRID(_pageID, _slotID);
    }
    
    void getRIDContent(int &pageID, int &slotID) const {
        pageID = _pageID;
        slotID = _slotID;
    }
    
    void setData(const char *data, int recordSize, const int pageID, const int slotID) {
        if (_data != nullptr) {
            free(_data);
        }
    
        _data = (char*)malloc(sizeof(char) * recordSize);
        memcpy(_data, data, (size_t)recordSize);
        _recordSize = recordSize;
        _pageID = pageID;
        _slotID = slotID;
    }
};


#endif //DATABASE_RM_RECORD_H
