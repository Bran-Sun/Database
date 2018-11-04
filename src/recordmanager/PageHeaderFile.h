//
// Created by 孙桢波 on 2018/11/4.
//

#ifndef DATABASE_PAGEHEADERFILE_H
#define DATABASE_PAGEHEADERFILE_H

#include "../utils/pagedef.h"

struct PageHeaderFile
{
    int recordSize; //4byte
    int recordEachPage;
    int pageNumber;
    
    int emptyPageList[PAGE_SIZE / 4 - 3];
};

#endif //DATABASE_PAGEHEADERFILE_H
