//
// Created by 孙桢波 on 2018/11/3.
//

#ifndef DATABASE_RID_H
#define DATABASE_RID_H


class RID
{
private:
    int _pageID, _slotID;
public:
    RID(int pageID, int slotID) {
        _pageID = pageID;
        _slotID = slotID;
    }
    
    void getRID(int &pageID, int &slotID) const {
        pageID = _pageID;
        slotID = _slotID;
    }
};


#endif //DATABASE_RID_H
