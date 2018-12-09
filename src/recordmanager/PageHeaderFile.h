//
// Created by 孙桢波 on 2018/11/4.
//

#ifndef DATABASE_PAGEHEADERFILE_H
#define DATABASE_PAGEHEADERFILE_H

#include "../utils/pagedef.h"

struct AttrInfoRead
{
    char attrName[256];
    int attrLength;
    AttrType attrType;
    bool isNull, isIndex;
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
