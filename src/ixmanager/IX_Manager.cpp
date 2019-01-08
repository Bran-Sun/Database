//
// Created by 孙桢波 on 2018/11/23.
//

#include "IX_Manager.h"
#include "IX_HeaderPage.h"
#include <sstream>

int IX_Manager::createIndex(const std::string &filename, int indexNo, AttrType attrtype, int attrLength)
{
    std::stringstream newName;
    newName << filename << "." << indexNo;
    int maxKeyPerPage = (IX_PAGE_SIZE - attrLength - 4 * 4) / (12 + attrLength);
    if (maxKeyPerPage == 0) {
        //printf("IX_Manager: attrlength is too large!\n");
        return -1;
    }
    
    //printf("creating index file %s...\n", newName.str().c_str());
    
    if (_fm->createFile(newName.str().c_str())) {
        //printf("creating index file %s succeed!\n", newName.str().c_str());
    } else {
        //printf("creating index file %s failed!\n", newName.str().c_str());
        return -1;
    }
    
    int fileID;
    if (_fm->openFile(newName.str().c_str(), fileID)) {
        //printf("open index ifle %s success!", newName.str().c_str());
    } else {
        //printf("open index file %s failed!\n", newName.str().c_str());
        return -1;
    }
    
    //初始化第一页
    int pageID = 0;
    int index;
    BufType bt = _bpm->allocPage(fileID, pageID, index, false, IX_PAGE_SIZE);
    
    IX_HeaderPage *header = (IX_HeaderPage*)bt;
    header->pageNumber = 1;
    header->recordNumber = 0;
    header->rootPos = 0;
    header->maxKeyPerPage = maxKeyPerPage;
    header->attrType = attrtype;
    header->attrlength = attrLength;
    header->emptyPage[0] = 0;
    
    _bpm->markDirty(index);
    _bpm->writeBack(index);
    _fm->closeFile(fileID);
    
    //printf("establishing header page in index file %s, close!\n", newName.str().c_str());
    
    return 0;
}

int IX_Manager::destroyIndex(const std::string &filename, int indexNo)
{
    std::stringstream newName;
    newName << filename << "." << indexNo;
    
    int suc = _fm->destroyFile(newName.str().c_str());
    if (suc == -1) {
        printf("cannot destroy index file %s\n", newName.str().c_str());
    } else {
        printf("destroy index file %s successfully\n", newName.str().c_str());
    }
    
    return suc;
}

int IX_Manager::openIndex(const std::string &filename, int indexNo, IX_IndexHandle &handle)
{
    std::stringstream newName;
    newName << filename << "." << indexNo;
    
    if (handle.isOpen()) {
        //printf("index file %s is already open!", newName.str().c_str());
        return 0;
    }
    
    int fileID;
    bool suc = _fm->openFile(newName.str().c_str(), fileID);
    if (!suc) {
        //printf("cannot open index file %s\n", newName.str().c_str());
        return -1;
    }
    
    handle.setFileID(fileID);
    
    //load header page
    int pageID = 0;
    int index = 0;
    BufType bt = _bpm->allocPage(fileID, pageID, index, true, IX_PAGE_SIZE);
    handle.init((IX_HeaderPage*)bt, _bpm);
    _bpm->release(index);
    
    return 0;
}

int IX_Manager::closeIndex(IX_IndexHandle &handle)
{
    if (!handle.isOpen()) {
        printf("file is not opened!\n");
        return -1;
    }
    
    if (handle.isHeaderModify()) {
        int index;
        int pageID = 0;
        BufType b = _bpm->getPage(handle.getFileID(),  pageID, index, IX_PAGE_SIZE);
        IX_HeaderPage *header = (IX_HeaderPage*)b;
        handle.setHeaderPage(header);
        _bpm->markDirty(index);
        _bpm->writeBack(index, IX_PAGE_SIZE);
    }
    handle.forcePages();
    
    return 0;
}
