//
// Created by 孙桢波 on 2018/11/25.
//

#include "pagedef.h"
#include <string>

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

bool compInt(void *p1, void* p2, ComOp op, int attrlength)
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

bool compBool(void *p1, void* p2, ComOp op, int attrlength)
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

bool compFloat(void *p1, void* p2, ComOp op, int attrlength)
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

bool compString(void *p1, void* p2, ComOp op, int attrlength)
{
    std::string value((char*)p1, attrlength);
    std::string compData((char*)p2, attrlength);
    
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

bool compChar(void *p1, void* p2, ComOp op, int attrlength)
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

bool TypeCompWithComOp(void *p1, void *p2, AttrType attrType, ComOp op, int attrlength)
{
    switch(attrType) {
        case INT:
            return compInt(p1, p2, op, attrlength);
        case BOOL:
            return compBool(p1, p2, op, attrlength);
        case FLOAT:
            return compFloat(p1, p2, op, attrlength);
        case STRING:
            return compString(p1, p2, op, attrlength);
        case CHAR:
            return compChar(p1, p2, op, attrlength);
    }
}