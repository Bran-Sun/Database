//
// Created by 孙桢波 on 2018/11/25.
//

#include "pagedef.h"
#include <string>


ComOp reverseCom(ComOp comOp) {
    switch (comOp) {
        case EQ_OP:
        case NE_OP:
            return comOp;
        case GT_OP:
            return LE_OP;
        case GE_OP:
            return LT_OP;
        case LE_OP:
            return GT_OP;
        case LT_OP:
            return GE_OP;
        case NO_OP:
            return NO_OP;
    }
}

int TypeComp(const void *p1, const void *p2, AttrType type, int attrlength) {
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
        case DATE:
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

bool compInt(const void *p1, const void *p2, ComOp op, int attrlength)
{
    int value = *(int*)p1;
    int compData = *(int*)p2;
    switch (op) {
        case EQ_OP:
            return value == compData;
        case LT_OP:
            return value < compData;
        case GT_OP:
            return value > compData;
        case LE_OP:
            return value <= compData;
        case GE_OP:
            return value >= compData;
        case NE_OP:
            return value != compData;
        case NO_OP:
            return true;
        
    }
}

bool compBool(const void *p1, const void *p2, ComOp op, int attrlength)
{
    bool value = *(bool*)p1;
    bool compData = *(bool*)p2;
    switch(op) {
        case EQ_OP:
            return value == compData;
        case NE_OP:
            return value != compData;
        default:
            return false;
    }
}

bool compFloat(const void *p1, const void *p2, ComOp op, int attrlength)
{
    float value = *(float*)p1;
    float compData = *(float*)p2;
    switch (op) {
        case EQ_OP:
            return value == compData;
        case LT_OP:
            return value < compData;
        case GT_OP:
            return value > compData;
        case LE_OP:
            return value <= compData;
        case GE_OP:
            return value >= compData;
        case NE_OP:
            return value != compData;
        case NO_OP:
            return true;
    }
    return false;
}

bool compString(const void *p1, const void *p2, ComOp op, int attrlength)
{
    int res = strncmp((char*)p1, (char*)p2, attrlength);
    switch (op) {
        case EQ_OP:
            return res == 0;
        case LT_OP:
            return res < 0;
        case GT_OP:
            return res > 0;
        case LE_OP:
            return res <= 0;
        case GE_OP:
            return res >= 0;
        case NE_OP:
            return res != 0;
        case NO_OP:
            return true;
    }
    return false;
}

bool compChar(const void *p1, const void *p2, ComOp op, int attrlength)
{
    char value = *(char*)p1;
    char compData = *(char*)p2;
    switch (op) {
        case EQ_OP:
            return value == compData;
        case LT_OP:
            return value < compData;
        case GT_OP:
            return value > compData;
        case LE_OP:
            return value <= compData;
        case GE_OP:
            return value >= compData;
        case NE_OP:
            return value != compData;
        case NO_OP:
            return true;
    }
}

bool TypeCompWithComOp(const void *p1, const void *p2, AttrType attrType, ComOp op, int attrlength)
{
    switch(attrType) {
        case INT:
            return compInt(p1, p2, op, attrlength);
        case BOOL:
            return compBool(p1, p2, op, attrlength);
        case FLOAT:
            return compFloat(p1, p2, op, attrlength);
        case DATE:
        case STRING:
            return compString(p1, p2, op, attrlength);
        case CHAR:
            return compChar(p1, p2, op, attrlength);
        case NUL:
            //TODO
            return true;
    }
}