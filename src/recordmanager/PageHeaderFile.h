//
// Created by 孙桢波 on 2018/11/4.
//

#ifndef DATABASE_PAGEHEADERFILE_H
#define DATABASE_PAGEHEADERFILE_H

#include "../utils/pagedef.h"

struct AttrInfoRead
{
    char attrName[ATTRNAME_MAX_LEN + 1];
    char foreignTb[TABLE_NAME_MAX_LENGTH + 1];
    char foreignIndex[ATTRNAME_MAX_LEN + 1];
    
    int attrLength;
    AttrType attrType;
    bool isNull, isIndex, isPrimary, isForeign;
};

struct PageHeaderFile
{
    int recordSize; //4byte
    int recordEachPage;
    int pageNumber;
    int attrNumber;
    int emptyPageHead;
    AttrInfoRead attributions[MAX_COL_NUM];
};

#endif //DATABASE_PAGEHEADERFILE_H
