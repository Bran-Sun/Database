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
    
    std::string buf;
    buf.assign(recordSize, '\0');
    
    for (auto &record: data) {  //insert Record
        buf.assign(recordSize, '\0');
        int point = RECORD_HEAD * 4;
        int head = 0;
        for (int i = 0; i < record.size(); i++) {
            if (record[i].isNull) {
                if (_attributions[i].isNull) {
                    head |= (1 << i);
                } else {
                    printf("this attributions can't insert!\n");
                }
            } else {
                if (_attributions[i].attrType == INT || _attributions[i].attrType == FLOAT) {
                    buf.replace(point, _attributions[i].attrLength, record[i].data);
                } else if (_attributions[i].attrLength < record[i].data.size()) {
                    printf("can't match: %d\n", i);
                    printf("this data is too long!\n");
                } else {
                    buf.replace(point, record[i].data.size(), record[i].data);
                }
            }
            point += _attributions[i].attrLength;
        }
        std::string tem((const char*)&head, 4);
        
        buf.replace(4, 4, tem);
        
        _rmHandle.insertRecord(buf.c_str(), rid);
        rids.push_back(rid);
    }
    
    for (int i = 0; i < _attributions.size(); i++) {
        if (_attributions[i].isIndex) {
            for (int j = 0; j < data.size(); j++) {
                _ixHandles.at(_attributions[i].attrName).insertEntry(data[j][i].data.c_str(), rids[j]);
            }
        }
    }
}

void TableHandle::del(RM_Record &record) {
    std::string tem = record._data.substr(RECORD_REFER * 4, 4);
    int refer = *(int*)(tem.c_str());
    printf("reference: %d\n", refer);
    if (refer > 0) {
        throw Error("Record cannot be delete, you need to delete foreignKey first!\n", Error::DELETE_ERROR);
    }
    RID rid = record.getRID();
    _rmHandle.deleteRecord(rid);
    const char *head = record._data.c_str();
    int offset = 0;
    for ( int i = 0; i < _attributions.size(); i++ )
    {
        if ( _attributions[i].isIndex)
        {
            _ixHandles.at(_attributions[i].attrName).deleteEntry(head + RECORD_HEAD * 4 + offset, record.getRID());
        }
        offset += _attributions[i].attrLength;
    }
}

void TableHandle::del(std::vector<WhereClause> &whereClause) {
    checkWhereValid(whereClause);
    
    std::vector<RM_Record> records;
    
    getWhereRecords(whereClause, records);
    
    int deleteNum = 0;
    for (auto &record: records) {
        std::string tem = record._data.substr(RECORD_REFER * 4, 4);
        int refer = *(int*)(tem.c_str());
        if (refer > 0) {
            printf("Record cannot be delete, you need to delete foreignKey first!\n");
            continue;
        }
        RID rid = record.getRID();
        _rmHandle.deleteRecord(rid);
    }
    
    for (auto &record: records)
    {
        const char *head = record._data.c_str();
        int offset = 0;
        for ( int i = 0; i < _attributions.size(); i++ )
        {
            if ( _attributions[i].isIndex)
            {
                _ixHandles.at(_attributions[i].attrName).deleteEntry(head + RECORD_HEAD * 4 + offset, record.getRID());
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

bool TableHandle::checkWhereValid(std::vector<WhereClause> &whereClause)
{
    return true;
}

bool TableHandle::getWhereRecords(std::vector<WhereClause> &whereClause, std::vector<RM_Record> &records) {
    WhereClause indexClause;
    AttrInfo indexInfo;
    int indexOffset;
    bool find = false;
    for (WhereClause &clause: whereClause) {
        int offset = 0;
        for ( auto &attr : _attributions )
        {
            if ((clause.left.col.indexName == attr.attrName) && (attr.isIndex) && (clause.right.isVal) && (!clause.right.useNull)) {
                if (!find) {
                    indexClause = clause;
                    indexInfo = attr;
                    indexOffset = offset;
                    find = true;
                } else if (indexInfo.attrLength < attr.attrLength) {
                    indexClause = clause;
                    indexInfo = attr;
                    indexOffset = offset;
                }
                break;
            }
            offset += attr.attrLength;
        }
    }
    
    if (!find)
    {
        RM_Iterator iter(_rmHandle, INT, 4, 0, GE_OP, nullptr);
        RM_Record recordIn;
        
        while ( iter.getNextRecord(recordIn) != -1 )
        {
            if ( _checkWhereClause(recordIn, whereClause))
            {
                records.push_back(recordIn);
            }
        }
    } else {
        if (!indexClause.right.isNull) //如果是 (= null) 就清空
        {
            IX_IndexScan iter(_ixHandles.at(indexInfo.attrName), indexInfo.attrType, indexInfo.attrLength,
                              indexClause.comOp, indexClause.right.value.c_str());
            std::vector<RID> rids;
            RID rid;
            RM_Record recordIn;
            while (iter.getNextEntry(rid) != -1) {
                rids.push_back(rid);
            }
            
            for (auto &r: rids) {
                _rmHandle.getRecord(r, recordIn);
                if (_checkWhereClause(recordIn, whereClause)) {
                    records.push_back(recordIn);
                }
            }
        }
    }
    return find;
}

void TableHandle::update(std::vector<WhereClause> &whereClause, std::vector<SetClause> &setClause)
{
    checkWhereValid(whereClause);
    _checkSetValid(setClause);
    
    std::vector<RM_Record> records;
    
    getWhereRecords(whereClause, records);
    
    printf("update record size: %lu\n", records.size());
    
    for (auto &record: records) {
        for (auto &clause: setClause) {
            int offset = 0;
            int index;
            for (index = 0; index < _attributions.size(); index++) {
                if (_attributions[index].attrName == clause.col) break;
                else offset += _attributions[index].attrLength;
            }
            
            if (_attributions[index].isIndex) {
                _ixHandles.at(_attributions[index].attrName).deleteEntry(record._data.c_str() + offset + 4 * RECORD_HEAD, record.getRID());
            }
            record._data.replace(offset + 4 * RECORD_HEAD, _attributions[index].attrLength, std::string()); //for clear()
            record._data.replace(offset + 4 * RECORD_HEAD, _attributions[index].attrLength, clause.value);
            if (_attributions[index].isIndex) {
                _ixHandles.at(_attributions[index].attrName).insertEntry(record._data.c_str() + offset + 4 * RECORD_HEAD, record.getRID());
            }
        }
        _rmHandle.updateRecord(record);
    }
}

bool TableHandle::_checkSetValid(std::vector<SetClause> &setClause)
{
    return true;
}

void TableHandle::_modifyWhereClause(std::vector<WhereClause> &whereClause)
{

}

void TableHandle::selectSingle(std::vector<Col> &selector, bool selectAll, std::vector<WhereClause> &whereClause)
{
    std::vector<std::vector<std::string>> data;
    
    _modifyWhereClause(whereClause);
    checkWhereValid(whereClause);
    
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
    
    WhereClause indexClause;
    AttrInfo indexInfo;
    int indexOffset;
    bool find = false;
    for (WhereClause &clause: whereClause) {
        int offset = 0;
        for ( auto &attr : _attributions )
        {
            if ((clause.left.col.indexName == attr.attrName) && (attr.isIndex) && (clause.right.isVal) && (!clause.right.useNull)) {
                if (!find) {
                    indexClause = clause;
                    indexInfo = attr;
                    indexOffset = offset;
                    find = true;
                } else if (indexInfo.attrLength < attr.attrLength) {
                    indexClause = clause;
                    indexInfo = attr;
                    indexOffset = offset;
                }
                break;
            }
            offset += attr.attrLength;
        }
    }
    
    
    
    
    if (!find) {
        RM_Iterator iter(_rmHandle, INT, 4, 0, GE_OP, nullptr); //no use GE
        RM_Record recordIn;
    
        while (iter.getNextRecord(recordIn) != -1) {
            if (_checkWhereClause(recordIn, whereClause)) {
                data.emplace_back();
                if (selectAll) {
                    int offset = 0;
                    for (auto &attr: _attributions)
                    {
                        data[data.size() - 1].push_back(recordIn._data.substr(RECORD_HEAD * 4 + offset, attr.attrLength));
                        offset += attr.attrLength;
                    }
                } else
                {
                    std::string tem;
                    for ( int i = 0; i < indexes.size(); i++ )
                    {
                        data[data.size() - 1].push_back(recordIn._data.substr(RECORD_HEAD * 4 + offsets[i], _attributions[indexes[i]].attrLength));
                    }
                }
            }
        }
    } else {
        if (!indexClause.right.isNull) //如果是 (= null) 就清空
        {
            IX_IndexScan iter(_ixHandles.at(indexInfo.attrName), indexInfo.attrType, indexInfo.attrLength,
                              indexClause.comOp, indexClause.right.value.c_str());
            std::vector<RID> rids;
            RID rid;
            RM_Record recordIn;
            while (iter.getNextEntry(rid) != -1) {
                rids.push_back(rid);
            }
            
            for (auto &r: rids) {
                _rmHandle.getRecord(r, recordIn);
                if (_checkWhereClause(recordIn, whereClause)) {
                    data.emplace_back();
                    if (selectAll) {
                        int offset = 0;
                        for (auto &attr: _attributions)
                        {
                            data[data.size() - 1].push_back(recordIn._data.substr(RECORD_HEAD * 4 + offset, attr.attrLength));
                            offset += attr.attrLength;
                        }
                    } else
                    {
                        std::string tem;
                        for ( int i = 0; i < indexes.size(); i++ )
                        {
                            data[data.size() - 1].push_back(recordIn._data.substr(RECORD_HEAD * 4 + offsets[i], _attributions[indexes[i]].attrLength));
                        }
                    }
                }
            }
        }
    }
    
    //print the result
    std::string splitLine;
    splitLine.assign(50, '=');
    printf("%s\n", splitLine.c_str());
    if (selectAll) {
        for (auto &attr: _attributions) {
            printf("%s\t", attr.attrName.c_str());
        }
    } else
    {
        for ( auto &i : indexes )
        {
            printf("%s\t", _attributions[i].attrName.c_str());
        }
    }
    printf("\n");
    printf("%s\n", splitLine.c_str());
    
    for (auto r: data) {
        for (int i = 0; i < _attributions.size(); i++) {
            if (_attributions[i].attrType == INT) {
                int *t = (int*)(r[i].c_str());
                printf("%d\t", t[0]);
            } else if (_attributions[i].attrType == FLOAT) {
                float *t = (float*)(r[i].c_str());
                printf("%.2f\t", t[0]);
            } else {
                printf("%s\t", r[i].c_str());
            }
        }
        printf("\n");
    }
    
    printf("%s\n", splitLine.c_str());
}

void TableHandle::insert(const std::vector<DataAttr> &data)
{
    RID rid;
    
    bool res = false;
    for (int i = 0; i < _attributions.size(); i++) {
        if (_attributions[i].isPrimary) {
            res = _ixHandles.at(_attributions[i].attrName).notInIndex(data[i].data.c_str());
        }
    }
    
    if (res) { //already have
        throw Error("Record have the same primary key with item in table!\n", Error::INSERT_ERROR);
    }
    
    int recordSize = _rmHandle.getRecordSize();
    std::string buf;
    
    buf.assign(recordSize, '\0');
    int point = RECORD_HEAD * 4;
    int head = 0;
    for ( int i = 0; i < data.size(); i++ )
    {
        if ( data[i].isNull )
        {
            if ( _attributions[i].isNull )
            {
                head |= (1 << i);
            } else
            {
                printf("this attributions can't insert!\n");
            }
        } else
        {
            if ( _attributions[i].attrType == INT || _attributions[i].attrType == FLOAT )
            {
                buf.replace(point, _attributions[i].attrLength, data[i].data);
            } else if ( _attributions[i].attrLength < data[i].data.size())
            {
                printf("can't match: %d\n", i);
                printf("this data is too long!\n");
            } else
            {
                buf.replace(point, data[i].data.size(), data[i].data);
            }
        }
        point += _attributions[i].attrLength;
    }
    std::string tem((const char *) &head, 4);
    buf.replace(4, 4, tem);
    
    _rmHandle.insertRecord(buf.c_str(), rid);
    
    for ( int i = 0; i < _attributions.size(); i++ )
    {
        if ( _attributions[i].isIndex )
        {
            _ixHandles.at(_attributions[i].attrName).insertEntry(data[i].data.c_str(), rid);
        }
    }
}

bool TableHandle::addForeign(const char *key) {
    IX_IndexScan iter(_ixHandles.at(_primaryKey.attrName), _primaryKey.attrType, _primaryKey.attrLength,
                      EQ_OP, key);
    RID rid;
    RM_Record recordIn;
    if (iter.getNextEntry(rid) == -1) {
        return false;
    }
    
    _rmHandle.getRecord(rid, recordIn);
    
    std::string tem = recordIn._data.substr(RECORD_REFER * 4, 4);
    int refer = *(int*)(tem.c_str());
    refer++;
    tem = std::string((const char*)&refer, 4);
    recordIn._data.replace(RECORD_REFER * 4, 4, tem);
    _rmHandle.updateRecord(recordIn);
    
    _rmHandle.getRecord(rid, recordIn);
    tem = recordIn._data.substr(RECORD_REFER * 4, 4);
    refer = *(int*)(tem.c_str());
    return true;
}

bool TableHandle::delForeign(const char *key) {
    IX_IndexScan iter(_ixHandles.at(_primaryKey.attrName), _primaryKey.attrType, _primaryKey.attrLength,
                      EQ_OP, key);
    RID rid;
    RM_Record recordIn;
    if (iter.getNextEntry(rid) == -1) {
        return false;
    }
    _rmHandle.getRecord(rid, recordIn);
    
    std::string tem = recordIn._data.substr(RECORD_REFER * 4, 4);
    int refer = *(int*)(tem.c_str());
    refer--;
    if (refer < 0) {
        printf("WARNING: refer will be negative!\n");
        return false;
    }
    tem = std::string((const char*)&refer, 4);
    recordIn._data.replace(RECORD_REFER * 4, 4, tem);
    
    _rmHandle.updateRecord(recordIn);
    
    return true;
}

void TableHandle::update(RM_Record &record, std::vector<SetClause> &setClause, std::vector<int> &indexes, std::vector<int> &offsets) {
    for (int i = 0; i < setClause.size(); i++) {
        printf("fuck: %d\n", *(int*)(record._data.c_str() + offsets[i] + 4 * RECORD_HEAD));
        if ( _attributions[indexes[i]].isIndex ) {
            _ixHandles.at(_attributions[indexes[i]].attrName).deleteEntry(record._data.c_str() + offsets[i] + 4 * RECORD_HEAD,
                                                                     record.getRID());
        }
        record._data.replace(offsets[i] + 4 * RECORD_HEAD, _attributions[indexes[i]].attrLength, std::string()); //for clear()
        record._data.replace(offsets[i] + 4 * RECORD_HEAD, _attributions[indexes[i]].attrLength, setClause[i].value);
        if ( _attributions[indexes[i]].isIndex ) {
            _ixHandles.at(_attributions[indexes[i]].attrName).insertEntry(record._data.c_str() + offsets[i] + 4 * RECORD_HEAD,
                                                                     record.getRID());
        }
    }
    _rmHandle.updateRecord(record);
}
