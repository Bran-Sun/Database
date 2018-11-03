//
// Created by 孙桢波 on 2018/11/3.
//

#ifndef DATABASE_RM_FILEHANDLE_H
#define DATABASE_RM_FILEHANDLE_H


class RM_FileHandle
{
private:
    int _fileID;
    
public:
    RM_FileHandle();
    ~RM_FileHandle();
    
    int getRecord(const RID &rid, RM_Record &record) const;
    int insertRecord(const char* data, RID &rid);
    int deleteRecord(RID &rid);
    int updateRecord(const RM_Record &record);
    int forcePages() const;
    
    void setFileID(int fileID) const;
};


#endif //DATABASE_RM_FILEHANDLE_H
