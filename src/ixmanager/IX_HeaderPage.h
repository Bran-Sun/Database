//
// Created by 孙桢波 on 2018/11/23.
//

#ifndef DATABASE_IM_HEADERPAGE_H
#define DATABASE_IM_HEADERPAGE_H

#include "../utils/pagedef.h"

#define IX_PAGE_SIZE 8192
#define RID_PER_BUCKET 32 //每个桶的rid最多数目

struct IX_HeaderPage
{
    int pageNumber; //总页数
    int recordNumber;   //总记录数
    int rootPos; //根节点位置
    int maxKeyPerPage; //每一页key最多的数目
    AttrType attrType;
    int attrlength;
    int emptyPage[IX_PAGE_SIZE / 4 - 6];
};

#endif //DATABASE_IM_HEADERPAGE_H
