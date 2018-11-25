//
// Created by 孙桢波 on 2018/11/25.
//

#include "pagedef.h"

int TypeComp(void *p1, void *p2, AttrType type, int attrlength) {
    char *a, *b;
    
    switch (type) {
        case INT:
            if (*(int*)p1 < *(int*)p2) return -1;
            else if (*(int*)p1 == *(int*)p2) return 0;
            else return 1;
        case FLOAT:
            if (*(float*)p1 < *(float*)p2) return -1;
            else if (*(float*)p1 == *(float*)p2) return 0;
            else return 1;
        case STRING:
            a = (char*)p1;
            b = (char*)p2;
            return strncmp(a, b, attrlength);
        case CHAR:
            if (*(char*)p1 < *(char*)p2) return -1;
            else if (*(char*)p1 == *(char*)p2) return 0;
            else return 1;
        default:
            return 0;
    }
}