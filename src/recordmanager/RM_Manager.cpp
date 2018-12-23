//
// Created by 孙桢波 on 2018/11/3.
//

#include "RM_Manager.h"
#include <sstream>

/*
 * @函数名createFile
 * @参数filename:文件名，根据文件名创建文件
 * @参数recordSize: 记录的大小
 * 返回:操作成功，返回0
 * 功能:创建文件，并在第一页注明文件记录长度
 */
int RM_Manager::createFile(const std::string &filename, std::vector<AttrInfo> attributes)
{
    printf("rm_manager: creating record file %s ...\n", filename.c_str());
    
    //recordSize: reference count(int), nullbit(int), attributions
    int recordSize = 0;
    for (int i = 0; i < attributes.size(); i++) {
        recordSize += attributes[i].attrLength;
    }
    recordSize += RECORD_HEAD * 4;    //reference count + nullbit
    
    if (recordSize > PAGE_SIZE) {
        printf("creating file failed! record size larger than page size\n");
        return -1;
    } //记录大小必须小于页的大小
    
    std::string newName;
    newName = filename + ".data" ;
    
    int fileID;
    _fm->createFile(newName.c_str());
    bool suc = _fm->openFile(newName.c_str(), fileID);
    if (!suc) return -1;
    
    printf("creating record file %s succeed!\n", filename.c_str());
    
    //初始化第一页
    int pageID = 0;
    int index;
    BufType bt = _bpm->allocPage(fileID, pageID, index, false);
    PageHeaderFile *header = (PageHeaderFile*)bt;
    header->recordSize = recordSize;
    int recordNum = (PAGE_SIZE - MAX_RECORD_PAGE / 8) / recordSize;
    if (recordNum > MAX_RECORD_PAGE) recordNum = MAX_RECORD_PAGE;
    header->recordEachPage = recordNum;
    header->pageNumber = 1;
    header->attrNumber  = (int)attributes.size();
    header->emptyPageHead = 0;
    for (int i = 0; i < attributes.size(); i++) {
        memcpy(header->attributions[i].attrName, attributes[i].attrName.c_str(), attributes[i].attrName.size() + 1);
        memcpy(header->attributions[i].foreignTb, attributes[i].foreignTb.c_str(), attributes[i].foreignTb.size() + 1);
        memcpy(header->attributions[i].foreignIndex, attributes[i].foreignIndex.c_str(), attributes[i].foreignIndex.size() + 1);
        header->attributions[i].attrLength = attributes[i].attrLength;
        header->attributions[i].attrType = attributes[i].attrType;
        header->attributions[i].isNull = attributes[i].isNull;
        header->attributions[i].isIndex = attributes[i].isIndex;
        header->attributions[i].isPrimary = attributes[i].isPrimary;
        header->attributions[i].isForeign = attributes[i].isForeign;
    }
    
    _bpm->markDirty(index);
    _bpm->writeBack(index);
    _fm->closeFile(fileID);
    return 0;
}

int RM_Manager::destroyFile(const std::string &filename)
{
    std::string newName;
    newName = filename + ".data" ;
    int suc = _fm->destroyFile(newName.c_str());
    return suc;
}

int RM_Manager::openFile(const std::string &filename, RM_FileHandle &handle)
{
    std::string newName;
    newName = filename + ".data" ;
    
    if (handle.isOpen()) {
        printf("rm_manager: data file already open!\n");
        return 1;
    }
    
    int fileID;
    bool suc = _fm->openFile(newName.c_str(), fileID);
    if (!suc) return -1;
    
    handle.setFileID(fileID);
    int pageID = 0;
    int index = 0;
    BufType bt = _bpm->allocPage(fileID, pageID, index, true);
    handle.init((PageHeaderFile*)bt, _bpm);
    _bpm->release(index);
    return 0;
}

int RM_Manager::closeFile(RM_FileHandle &handle)
{
    if (!handle.isOpen()) return 0;
    if (handle.isHeaderModify()) {
        int index;
        int pageID = 0;
        BufType b = _bpm->allocPage(handle.getFileID(), pageID, index, false);
        PageHeaderFile *bt = (PageHeaderFile*)b;
        handle.setHeaderPage(bt);
        _bpm->markDirty(index);
        _bpm->writeBack(index);
    }
    handle.forcePages();
    _fm->closeFile(handle.getFileID());
    handle.setClose();
    return 0;
}