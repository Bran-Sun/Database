//
// Created by 孙桢波 on 2018/12/6.
//

#include <sstream>
#include "TableHandle.h"

TableHandle::TableHandle()
{
    _open = false;
}

TableHandle::TableHandle(const std::string &dbName, const std::string &relName, std::shared_ptr<RM_Manager> rm, std::shared_ptr<IX_Manager> ix)
{
    _rm = rm;
    _ix = ix;
    
    _tableName = relName;
    _dbName = dbName;
    _rm->openFile(_dbName + "/" + _tableName, _rmHandle);
    _attributions = _rmHandle.getAttrInfo();
    
    _getPrimaryKey();
    
    _openIndex();
    _open = true;
}

TableHandle::TableHandle(const std::string &dbName, const std::string &relName, std::vector<AttrInfo> attributes, std::shared_ptr<RM_Manager> rm,
                         std::shared_ptr<IX_Manager> ix)
{
    _tableName = relName;
    _dbName = dbName;
    _rm = rm;
    _ix = ix;
    
    _attributions  = attributes;
    _getPrimaryKey();
    
    _rm->createFile(_dbName + "/" + _tableName, _attributions); //create record table
    
    for (int i = 0; i < _attributions.size(); i++) {
        if (_attributions[i].isIndex) {
            _ix->createIndex(_dbName + "/" + _tableName, i, _attributions[i].attrType, _attributions[i].attrLength);
            _ixHandles.emplace(std::piecewise_construct,
                               std::forward_as_tuple(_attributions[i].attrName),
                               std::forward_as_tuple());
            _ix->openIndex(_dbName + "/" + _tableName, i, _ixHandles.at(_attributions[i].attrName));
        }
    }
    
    _rm->openFile(_dbName + "/" + _tableName, _rmHandle);
    
    _open = true;
}

int TableHandle::dropTable()
{
    _rm->closeFile(_rmHandle);
    
    for (auto iter = _ixHandles.begin(); iter != _ixHandles.end(); iter++) {
        _ix->closeIndex(iter->second);
    }
    
    _rm->destroyFile(_tableName);
    for (int i = 0; i < _attributions.size(); i++) {
        if (_attributions[i].isIndex) {
            _ix->destroyIndex(_tableName.c_str(), i);
        }
    }
    
    _ixHandles.clear();
    _attributions.clear();
    
    _open = false;
    
    return 0;
}

int TableHandle::createIndex(const std::string &attrName)
{
    auto find = _ixHandles.find(attrName);
    if (find != _ixHandles.end()) {
        printf("tbHandle: index has already created!\n");
        return -1;
    }
    
    _ixHandles.emplace(std::piecewise_construct,
                       std::forward_as_tuple(attrName),
                       std::forward_as_tuple());
    
    int indexNo;
    for (indexNo = 0; indexNo < _attributions.size(); indexNo++)
        if  (_attributions[indexNo].attrName == attrName) {
            break;
        }
    _ix->createIndex(_dbName + "/" + _tableName, indexNo, _attributions[indexNo].attrType, _attributions[indexNo].attrLength);
    _ix->openIndex(_dbName + "/" + _tableName, indexNo, _ixHandles.at(attrName));
    
    _rmHandle.addIndex(indexNo);
    _attributions[indexNo].isIndex = true;
    
    return 0;
}

int TableHandle::dropIndex(const std::string &attrName)
{
    auto find =  _ixHandles.find(attrName);
    if (find == _ixHandles.end()) {
        printf("tbHandle: table doesn't have this index!\n");
        return -1;
    }
    
    int indexNo;
    for (indexNo = 0; indexNo < _attributions.size(); indexNo++)
        if (_attributions[indexNo].attrName == attrName) {
            break;
        }
    if (indexNo >= _attributions.size()) {
        printf("no such index\n");
        return -1;
    }
    
    if (_attributions[indexNo].isPrimary || _attributions[indexNo].isForeign) {
        printf("cannot drop such index\n");
        return -1;
    }
    
    _ix->closeIndex(_ixHandles.at(attrName));
    _ix->destroyIndex(_dbName + "/" + _tableName, indexNo);
    _ixHandles.erase(attrName);
    
    _attributions[indexNo].isIndex = false;
    _rmHandle.dropIndex(indexNo);
    
    return 0;
}

int TableHandle::close()
{
    _rm->closeFile(_rmHandle);
    
    for (auto iter = _ixHandles.begin(); iter != _ixHandles.end(); iter++) {
        _ix->closeIndex(iter->second);
    }
    
    _ixHandles.clear();
    _attributions.clear();
    
    _open = false;
    return 0;
}

void TableHandle::_openIndex()
{
    for (int i = 0; i < _attributions.size(); i++) {
        if (_attributions[i].isIndex) {
            _ixHandles.emplace(std::piecewise_construct,
                               std::forward_as_tuple(_attributions[i].attrName),
                               std::forward_as_tuple());
            _ix->openIndex(_dbName + "/" + _tableName, i, _ixHandles.at(_attributions[i].attrName));
        }
    }
}

void TableHandle::_getPrimaryKey()
{
    for (auto &attr: _attributions) {
        if (attr.isPrimary) {
            _primaryKey = attr;
            return;
        }
    }
}

void TableHandle::insert(const std::vector<std::vector<DataAttr>> &data)
{
    RID rid;
    
    std::vector<RID> rids;
    
    int recordSize = _rmHandle.getRecordSize();
    
    char *buf = (char*)malloc(recordSize * sizeof(char));
    int *head = (int*)buf;
    
    for (auto &record: data) {  //insert Record
        memset(buf, 0, (size_t)recordSize);
        int point = RECORD_HEAD * 4;
        
        for (int i = 0; i < record.size(); i++) {
            if (record[i].isNull) {
                if (_attributions[i].isNull) {
                    head[RECORD_NULLBIT] |= (1 << i);
                } else {
                    printf("this attributions can't insert!\n");
                }
            } else {
                if (_attributions[i].attrType == INT) {
                    stringstream r(record[i].data);
                    int tem;
                    r >> tem;
                    ((int*)(buf + point))[0] = tem;
                } else if (_attributions[i].attrType == FLOAT){
                    stringstream r(record[i].data);
                    float tem;
                    r >> tem;
                    ((float*)(buf + point))[0] = tem;
                } else if (_attributions[i].attrLength < record[i].data.size()) {
                    printf("this data is too long!\n");
                } else {
                    strncpy(buf + point, record[i].data.c_str(), record[i].data.size());
                }
            }
            point += _attributions[i].attrLength;
        }
        
        _rmHandle.insertRecord(buf, rid);
        rids.push_back(rid);
    }
    
    for (int i = 0; i < _attributions.size(); i++) {
        if (_attributions[i].isIndex) {
            for (int j = 0; j < data.size(); j++) {
                memset(buf, 0, (size_t)_attributions[i].attrLength);
                strncpy(buf, data[j][i].data.c_str(), data[j][i].data.size());
                _ixHandles.at(_attributions[i].attrName).insertEntry(buf, rids[j]);
            }
        }
    }
}


void TableHandle::del(std::vector<WhereClause> &whereClause) {
    _checkWhereValid(whereClause);
    
    std::vector<RM_Record> records;
    int recordSize = _rmHandle.getRecordSize();
    char *buf = (char*)malloc(recordSize * sizeof(char));
    
    RM_Iterator iter(_rmHandle, INT, 4, 0, GE_OP, nullptr);
    RM_Record recordIn;
    
    while (iter.getNextRecord(recordIn) != -1) {
        if (_checkWhereClause(recordIn, whereClause)) {
            records.push_back(recordIn);
        }
    }
    
    for (auto &record: records) {
        RID rid = record.getRID();
        _rmHandle.deleteRecord(rid);
    }
    
    for (auto &record: records)
    {
        const char *head = record._data.c_str();
        RID rid = record.getRID();
        int offset = 0;
        for ( int i = 0; i < _attributions.size(); i++ )
        {
            if ( _attributions[i].isIndex)
            {
                _ixHandles.at(_attributions[i].attrName).deleteEntry(head + RECORD_HEAD * 4 + offset, rid);
            }
            offset += _attributions[i].attrLength;
        }
    }
}


bool TableHandle::_checkWhereClause(RM_Record &record, std::vector<WhereClause> &whereClause)
{
    std::string data = record.getData();
    const char *head = data.c_str();
    for (auto &clause: whereClause) {
        int index, offset = 0;
        for (index = 0; index < _attributions.size(); index++) {
            if (clause.left.col.indexName == _attributions[index].attrName) break;
            else offset += _attributions[index].attrLength;
        }
        
        if (clause.right.isVal) {
            if (clause.right.useNull) {
                bool isNull = (((((int*)head)[RECORD_NULLBIT] >> index) & 1) == 1);
                if (isNull != clause.right.isNull) return false;
            } else {
                bool result = TypeCompWithComOp(head + RECORD_HEAD * 4 + offset, clause.right.value.c_str(), clause.right.type, clause.comOp, _attributions[index].attrLength);
                if (!result) return false;
            }
        } else {
            int index2, offset2 = 0;
            for (index2 = 0; index2 < _attributions.size(); index2++) {
                if (clause.right.col.indexName == _attributions[index2].attrName) break;
                else offset2 += _attributions[index2].attrLength;
            }
            bool result = TypeCompWithComOp(head + RECORD_HEAD * 4 + offset, head + RECORD_HEAD * 4 + offset2, _attributions[index].attrType, clause.comOp, _attributions[index].attrLength);
            if (!result) return false;
        }
    }
    return true;
}

bool TableHandle::_checkWhereValid(std::vector<WhereClause> &whereClause)
{
    return true;
}

void TableHandle::update(std::vector<WhereClause> &whereClause, std::vector<SetClause> &setClause)
{
    _checkWhereValid(whereClause);
    _checkSetValid(setClause);
    
    std::vector<RM_Record> records;
    int recordSize = _rmHandle.getRecordSize();
    char *buf = (char*)malloc(recordSize * sizeof(char));
    
    RM_Iterator iter(_rmHandle, INT, 4, 0, GE_OP, nullptr);
    RM_Record recordIn;
    
    while (iter.getNextRecord(recordIn) != -1) {
        if (_checkWhereClause(recordIn, whereClause)) {
            records.push_back(recordIn);
        }
    }
    
    for (auto &record: records) {
        for (auto &clause: setClause) {
            int offset = 0;
            int index;
            for (index = 0; index < _attributions.size(); index++) {
                if (_attributions[index].attrName == clause.col) break;
                else offset += _attributions[index].attrLength;
            }
            
            if (_attributions[index].isIndex) {
                _ixHandles.at(_attributions[index].attrName).deleteEntry(record._data.c_str() + offset, record.getRID());
            }
            record._data.replace(offset, _attributions[index].attrLength, clause.value);
            if (_attributions[index].isIndex) {
                _ixHandles.at(_attributions[index].attrName).insertEntry(record._data.c_str() + offset, record.getRID());
            }
        }
        _rmHandle.updateRecord(record);
    }
}

bool TableHandle::_checkSetValid(std::vector<SetClause> &setClause)
{
    return true;
}

void TableHandle::selectSingle(std::vector<Col> &selector, bool selectAll, std::vector<WhereClause> &whereClause)
{
    std::vector<std::string> data;
    
    _checkWhereValid(whereClause);
    
    //prepare selector
    std::vector<int> indexes;
    std::vector<int> offsets;
    for (auto &col: selector) {
        int offset = 0;
        for (int i = 0; i < _attributions.size(); i++) {
            if (_attributions[i].attrName == col.indexName) {
                indexes.push_back(i);
                offsets.push_back(offset);
                break;
            } else {
                offset += _attributions[i].attrLength;
            }
        }
    }
    
    int recordSize = _rmHandle.getRecordSize();
    char *buf = (char*)malloc(recordSize * sizeof(char));
    
    RM_Iterator iter(_rmHandle, INT, 4, 0, GE_OP, nullptr);
    RM_Record recordIn;
    
    while (iter.getNextRecord(recordIn) != -1) {
        if (_checkWhereClause(recordIn, whereClause)) {
            if (selectAll) {
                data.push_back(recordIn._data);
            } else
            {
                std::string tem;
                for ( int i = 0; i < indexes.size(); i++ )
                {
                    tem += recordIn._data.substr(offsets[i], _attributions[indexes[i]].attrLength);
                }
                data.push_back(tem);
            }
        }
    }
}