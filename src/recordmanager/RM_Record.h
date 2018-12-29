//
// Created by 孙桢波 on 2018/11/3.
//

#ifndef DATABASE_RM_RECORD_H
#define DATABASE_RM_RECORD_H

#include "RID.h"
#include <cstdlib>
#include <string>

class RM_Record
{
public:
    std::string _data;
    int _recordSize;
    int _pageID, _slotID;
    
public:
    RM_Record() {
        _recordSize = 0;
    }
    
    std::string getData() const{
        return _data;
    }
    
    void getRid(RID &rid) const {
        rid.setRID(_pageID, _slotID);
    }
    
    void setData(const char *data, int recordSize, const RID *rid) {
        _data = std::string(data, (unsigned long)recordSize);
        _recordSize = recordSize;
        rid->getRID(_pageID, _slotID);
    }
    
    void getRIDContent(int &pageID, int &slotID) const {
        pageID = _pageID;
        slotID = _slotID;
    }
    
    RID getRID() const {
        return RID(_pageID, _slotID);
    }
    
    void setData(const char *data, int recordSize, const int pageID, const int slotID) {
        _data = std::string(data, (unsigned long)recordSize);
        _recordSize = recordSize;
        _pageID = pageID;
        _slotID = slotID;
    }
};


#endif //DATABASE_RM_RECORD_H
