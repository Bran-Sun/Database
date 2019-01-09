#include <utility>

//
// Created by 孙桢波 on 2018/12/6.
//

#include "DatabaseHandle.h"

DatabaseHandle::DatabaseHandle() {
    _bpm = nullptr;
    _fm = nullptr;
    _rm = nullptr;
    _ix = nullptr;
    _fileID = -1;
    _open = false;
    _modifyDbf = false;
}

int DatabaseHandle::close() {
    if ( !_open ) {
        printf("database is already close!\n");
        return -1;
    }
    if ( _modifyDbf ) _modifyDBFile();
    
    for ( auto iter = _tableHandles.begin(); iter != _tableHandles.end(); iter++ ) {
        iter->second.close();
    }
    
    _fm->closeFile(_fileID);
    
    _tableHandles.clear();
    _tableNames.clear();
    _fm = nullptr;
    _bpm = nullptr;
    _rm = nullptr;
    _ix = nullptr;
    _fileID = -1;
    _open = false;
    _dbName = "";
    _modifyDbf = false;
    return 0;
}

int DatabaseHandle::open(const std::string &filename, std::shared_ptr<FileManager> fm,
                         std::shared_ptr<BufPageManager> bpm) {
    if ( _dbName == filename ) {
        printf("dbhandle: database is already open!\n");
        return 1;
    }
    
    if ( _open ) {
        close(); //open another
    }
    
    _dbName = filename;
    _fm = fm;
    _bpm = bpm;
    _rm = std::make_shared<RM_Manager>(RM_Manager(_fm, _bpm));
    _ix = std::make_shared<IX_Manager>(IX_Manager(_fm, _bpm));
    
    std::string dbname = filename + "/" + filename + ".dbf";
    
    int pageID, index;
    
    if ( !_fm->openFile(dbname.c_str(), _fileID)) {
        printf("dbhanlde: cannot open dbf file!\n");
        return -1;
    }
    
    //load dbf file
    pageID = 0;
    BufType header = _bpm->allocPage(_fileID, pageID, index, true);
    DBHeadPage *headerPage = (DBHeadPage *) header;
    int tableNum = headerPage->tableNum;
    for ( int i = 0; i < tableNum; i++ ) {
        _tableNames.emplace(headerPage->tables[i].tableName);
    }
    _bpm->release(index);
    _open = true;
    
    return 0;
}

void DatabaseHandle::_modifyDBFile() {
    int pageID = 0, index;
    BufType header = _bpm->allocPage(_fileID, pageID, index);
    DBHeadPage *headerPage = (DBHeadPage *) header;
    headerPage->tableNum = (int) _tableNames.size();
    int cnt = 0;
    for ( auto &i: _tableNames ) {
        memcpy(headerPage->tables[cnt].tableName, i.c_str(), i.size() + 1);
        cnt++;
    }
    printf("TableNum: %lu\n", _tableNames.size());
    _bpm->markDirty(index);
    _bpm->writeBack(index);
    _bpm->release(index);
}

bool DatabaseHandle::_checkAttrInfo(const std::vector<AttrInfo> &attributes) {
    for ( const auto &info: attributes ) {
        if ( info.attrName.length() > ATTRNAME_MAX_LEN ) {
            return false;
        }
        
        if ( info.isForeign ) {
            if ( _tableNames.find(info.foreignTb) == _tableNames.end()) {  //如果不存在这个表
                return false;
            }
            
            if ( _tableHandles.find(info.foreignTb) == _tableHandles.end()) {    //打开这个表
                _tableHandles.emplace(std::piecewise_construct,
                                      std::forward_as_tuple(info.foreignTb),
                                      std::forward_as_tuple(_dbName, info.foreignTb, _rm, _ix));
            }
            
            AttrInfo p = _tableHandles.at(info.foreignTb).getPrimaryKey();
            if ( p.attrName != info.foreignIndex || p.attrType != info.attrType || p.attrLength != info.attrLength ) {
                return false;
            }
        }
    }
    return true;
}

int DatabaseHandle::createTable(const std::string &relName, std::vector<AttrInfo> &attributes) {
    //first process the attributions
    for ( auto &at: attributes ) {
        if ( at.isPrimary || at.isForeign ) {
            at.isIndex = true;
        }
        
        if ( at.attrType == INT || at.attrType == FLOAT ) {
            at.attrLength = 4;
        }
    }
    
    if ( !_open ) {
        printf("dbHandle: table bot open!\n");
        return -1;
    }
    
    auto find = _tableNames.find(relName);
    if ( find != _tableNames.end()) {
        printf("dbHandle: table already exists!\n");
        return -1;
    }
    
    if ( !_checkAttrInfo(attributes)) {
        printf("dbHandle: attributes format error(maybe foreign key)\n");
        return -1;
    }
    
    _tableHandles.emplace(std::piecewise_construct,
                          std::forward_as_tuple(relName),
                          std::forward_as_tuple(_dbName, relName, attributes, _rm, _ix));
    _tableNames.emplace(relName);
    _modifyDbf = true;
    return 0;
}

int DatabaseHandle::dropTable(const std::string &relName) {
    if ( !_open ) {
        printf("database not open!\n");
        return -1;
    }
    
    auto find = _tableNames.find(relName);
    if ( find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return -1;
    }
    
    _tableHandles.at(relName).dropTable();
    _tableHandles.erase(relName);
    _tableNames.erase(relName);
    _modifyDbf = true;
    return 0;
}

int DatabaseHandle::createIndex(const std::string &relName, const std::string &attrName) {
    if ( !_open ) {
        printf("dbHandle: database not open\n");
        return -1;
    }
    
    auto find = _tableNames.find(relName);
    if ( find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return -1;
    }
    
    auto openFind = _tableHandles.find(relName);
    if ( openFind == _tableHandles.end()) {
        _tableHandles.emplace(std::piecewise_construct,
                              std::forward_as_tuple(relName),
                              std::forward_as_tuple(_dbName, relName, _rm, _ix));
    }
    int result = _tableHandles.at(relName).createIndex(attrName);
    
    return result;
}

int DatabaseHandle::dropIndex(const std::string &relName, const std::string &attrName) {
    auto find = _tableNames.find(relName);
    if ( find == _tableNames.end()) {
        printf("dbHandle: database not exists!\n");
        return -1;
    }
    
    auto openFind = _tableHandles.find(relName);
    if ( openFind == _tableHandles.end()) {
        _tableHandles.emplace(std::piecewise_construct,
                              std::forward_as_tuple(relName),
                              std::forward_as_tuple(_dbName, relName, _rm, _ix));
    }
    int result = _tableHandles.at(relName).dropIndex(attrName);
    return result;
}

/* not use
int DatabaseHandle::load(std::string &relName, std::string &filename)
{
    auto find = _tableNames.find(relName);
    if (find == _tableNames.end()) {
        printf("dbHandle: database not exists!\n");
        return -1;
    }
    
    auto openFind = _tableHandles.find(relName);
    if (openFind == _tableHandles.end()) {
        _tableHandles.emplace(relName, relName, _rm, _ix);
    }
    int result = _tableHandles.at(relName)->loadData(filename);
    
    return result;
}*/

DatabaseHandle::~DatabaseHandle() {
    if ( _open ) close();
}

std::vector<AttrInfo> DatabaseHandle::getRecordInfo(const std::string &tbName) {
    auto find = _tableNames.find(tbName);
    if ( find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return std::vector<AttrInfo>();
    }
    
    auto openFind = _tableHandles.find(tbName);
    if ( openFind == _tableHandles.end()) {
        _tableHandles.emplace(std::piecewise_construct,
                              std::forward_as_tuple(tbName),
                              std::forward_as_tuple(_dbName, tbName, _rm, _ix));
    }
    
    return _tableHandles.at(tbName).getAttributions();
}

void DatabaseHandle::insert(const std::string &tbName, const std::vector<std::vector<DataAttr>> &data) {
    _openTable(tbName);
    std::vector<AttrInfo> attrInfo = getRecordInfo(tbName);
    
    std::vector<int> indexes;
    for ( int i = 0; i < attrInfo.size(); i++ ) {
        if ( attrInfo[i].isForeign ) {
            indexes.push_back(i);
        }
    }
    
    for ( auto i: indexes ) {
        _openTable(attrInfo[i].foreignTb);
    }
    
    int errorNum = 0;
    for ( auto &single_data: data ) {
        //_insertSingleData(tbName, single_data, offsets, indexes);
        if (!_checkInsert(attrInfo, single_data)) {
            errorNum += 1;
            continue;
        }
        
        bool foreignSuccess = true;
        int i;
        for ( i = 0; i < indexes.size(); i++ ) {
            if ( !_tableHandles.at(attrInfo[indexes[i]].foreignTb).addForeign(single_data[indexes[i]].data.data())) {
                foreignSuccess = false;
                break;
            }
        }
        
        if ( foreignSuccess ) {
            try {
                _tableHandles.at(tbName).insert(single_data);
            } catch ( const Error &e ) {
                printf("%s", e.what());
                if ( e.getErrorType() == Error::INSERT_ERROR ) {
                    for ( int i = 0; i < indexes.size(); i++ ) {
                        _tableHandles.at(attrInfo[indexes[i]].foreignTb).delForeign(
                                single_data[indexes[i]].data.data());
                    }
                }
                errorNum++;
            }
        } else {
            for ( i = i - 1; i >= 0; i-- ) {
                _tableHandles.at(attrInfo[indexes[i]].foreignTb).delForeign(single_data[indexes[i]].data.data());
            }
            errorNum++;
        }
    }
    
    printf(">> insert %lu records, error records: %d\n", data.size() - errorNum, errorNum);
}

void DatabaseHandle::del(const std::string &tbName, std::vector<WhereClause> &whereClause) {
    auto find = _tableNames.find(tbName);
    if ( find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return;
    }
    _openTable(tbName);
    std::vector<AttrInfo> attrInfo = getRecordInfo(tbName);
    
    _tableHandles.at(tbName).checkWhereValid(whereClause);
    std::vector<RM_Record> records;
    _tableHandles.at(tbName).getWhereRecords(whereClause, records);
    
    if ( records.size() == 0 ) {
        printf(">> delete 0 items\n");
        return;
    }
    
    std::vector<int> indexes, offsets;
    int offset = 0;
    for ( int i = 0; i < attrInfo.size(); i++ ) {
        if ( attrInfo[i].isForeign ) {
            indexes.push_back(i);
            offsets.push_back(offset);
        }
        offset += attrInfo[i].attrLength;
    }
    
    for ( auto i: indexes ) {
        _openTable(attrInfo[i].foreignTb);
    }
    
    for ( auto record: records ) {
        try {
            _tableHandles.at(tbName).del(record);
            for ( int i = 0; i < indexes.size(); i++ ) {
                _tableHandles.at(attrInfo[indexes[i]].foreignTb).delForeign(
                        record._data.data() + RECORD_HEAD * 4 + offsets[i]);
            }
        } catch ( const Error &e ) {
            printf("%s", e.what());
            if ( e.getErrorType() == Error::DELETE_ERROR ) {
                continue;
            }
        }
    }
}

void DatabaseHandle::update(const std::string &tbName, std::vector<WhereClause> &whereClause,
                            std::vector<SetClause> &setClause) {
    auto find = _tableNames.find(tbName);
    if ( find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return;
    }
    _openTable(tbName);
    
    std::vector<AttrInfo> attrInfo = getRecordInfo(tbName);
    //TODO _checkSetClause()
    
    std::vector<int> indexes, offsets;
    for ( auto &clause: setClause ) {
        int offset = 0;
        for ( int i = 0; i < attrInfo.size(); i++ ) {
            if ( attrInfo[i].attrName == clause.col ) {
                indexes.push_back(i);
                offsets.push_back(offset);
                break;
            }
            offset += attrInfo[i].attrLength;
        }
    }
    
    _tableHandles.at(tbName).checkWhereValid(whereClause);
    std::vector<RM_Record> records;
    _tableHandles.at(tbName).getWhereRecords(whereClause, records);
    
    if ( records.size() == 0 ) {
        printf(">>update 0 items\n");
        return;
    }
    
    for ( auto i : indexes ) {
        if ( attrInfo[i].isForeign ) {
            _openTable(attrInfo[i].foreignTb);
        }
    }
    
    int errorNum = 0;
    for ( auto &record: records ) {
        int i = 0;
        bool success = true;
        for ( i = 0; i < setClause.size(); i++ ) {
            if ( attrInfo[indexes[i]].isForeign ) {
                success = _tableHandles.at(attrInfo[indexes[i]].foreignTb).delForeign(
                        record._data.c_str() + RECORD_HEAD * 4 + offsets[i]);
                if ( !success ) break;
                success = _tableHandles.at(attrInfo[indexes[i]].foreignTb).addForeign(setClause[i].value.c_str());
                if ( !success ) {
                    _tableHandles.at(attrInfo[indexes[i]].foreignTb).addForeign(
                            record._data.c_str() + RECORD_HEAD * 4 + offsets[i]);
                    break;
                }
            }
        }
        
        if ( !success ) {
            printf("update not success!\n");
            for ( i = i - 1; i >= 0; i-- ) {
                _tableHandles.at(attrInfo[indexes[i]].foreignTb).delForeign(setClause[i].value.c_str());
                _tableHandles.at(attrInfo[indexes[i]].foreignTb).addForeign(
                        record._data.c_str() + RECORD_HEAD * 4 + offsets[i]);
            }
            errorNum++;
            continue;
        }
        
        try {
            _tableHandles.at(tbName).update(record, setClause, indexes, offsets);
        } catch ( const Error &e ) {
            printf("%s", e.what());
            //assert never will occur bugs while update
            continue;
        }
    }
    
    printf(">>update %lu items\n", records.size() - errorNum);
}

void DatabaseHandle::select(std::vector<std::string> &tbList, std::vector<Col> &selector, bool selectAll,
                            std::vector<WhereClause> &whereClause) {
    for ( auto &tbName: tbList ) {
        _openTable(tbName);
    }
    
    if ( tbList.size() == 1 ) {
        _tableHandles.at(tbList[0]).selectSingle(selector, selectAll, whereClause);
    } else if ( tbList.size() == 2 ) {
        _selectDouble(tbList, selector, selectAll, whereClause);
    }
    //TODO multi table
}

void DatabaseHandle::_openTable(const std::string tbName) {
    auto find = _tableNames.find(tbName);
    if ( find == _tableNames.end()) {
        printf("dbHandle: table not exists!\n");
        return;
    }
    
    auto openFind = _tableHandles.find(tbName);
    if ( openFind == _tableHandles.end()) {
        _tableHandles.emplace(std::piecewise_construct,
                              std::forward_as_tuple(tbName),
                              std::forward_as_tuple(_dbName, tbName, _rm, _ix));
    }
}

void DatabaseHandle::_selectDouble(std::vector<std::string> &tbList, std::vector<Col> &selector, bool selectAll,
                                   std::vector<WhereClause> &whereClause) {
    //classify whereClause
    std::vector<WhereClause> where1, where2, whereCommon;
    for ( auto &clause: whereClause ) {
        if ( clause.right.isVal || (clause.left.col.tbName == clause.right.col.tbName)) {
            if ( clause.left.col.tbName == tbList[0] ) {
                where1.push_back(clause);
            } else {
                where2.push_back(clause);
            }
        } else {
            whereCommon.push_back(clause);
        }
    }
    
    std::vector<AttrInfo> attrInfo1 = getRecordInfo(tbList[0]);
    std::vector<AttrInfo> attrInfo2 = getRecordInfo(tbList[1]);
    
    std::vector<int> sindexes1, sindexes2;
    std::vector<int> soffsets1, soffsets2;
    
    if ( selectAll ) {
        int offset = 0;
        for ( int i = 0; i < attrInfo1.size(); i++ ) {
            sindexes1.push_back(i);
            soffsets1.push_back(offset);
            offset += attrInfo1[i].attrLength;
        }
        
        offset = 0;
        for ( int i = 0; i < attrInfo2.size(); i++ ) {
            sindexes2.push_back(i);
            soffsets2.push_back(offset);
            offset += attrInfo2[i].attrLength;
        }
    } else {
        for ( auto &col: selector ) {
            if ( col.tbName == tbList[0] ) {
                int offset = 0;
                for ( int i = 0; i < attrInfo1.size(); i++ ) {
                    if ( attrInfo1[i].attrName == col.indexName ) {
                        sindexes1.push_back(i);
                        soffsets1.push_back(offset);
                        break;
                    } else {
                        offset += attrInfo1[i].attrLength;
                    }
                }
            } else {
                int offset = 0;
                for ( int i = 0; i < attrInfo2.size(); i++ ) {
                    if ( attrInfo2[i].attrName == col.indexName ) {
                        sindexes2.push_back(i);
                        soffsets2.push_back(offset);
                        break;
                    } else {
                        offset += attrInfo2[i].attrLength;
                    }
                }
            }
        }
    }
    
    std::vector<std::vector<std::string>> data;
    
    _tableHandles.at(tbList[0]).checkWhereValid(where1);
    std::vector<RM_Record> records1;
    _tableHandles.at(tbList[0]).getWhereRecords(where1, records1);
    
    _tableHandles.at(tbList[1]).checkWhereValid(where2);
    std::vector<RM_Record> records2;
    _tableHandles.at(tbList[1]).getWhereRecords(where2, records2);
    
    std::vector<int> indexes1, indexes2, offsets1, offsets2;
    for ( auto &clause: whereCommon ) {
        if ( clause.left.col.tbName == tbList[0] ) {
            int offset = 0;
            for ( int i = 0; i < attrInfo1.size(); i++ ) {
                if ( attrInfo1[i].attrName == clause.left.col.indexName ) {
                    indexes1.push_back(i);
                    offsets1.push_back(offset);
                }
                offset += attrInfo1[i].attrLength;
            }
            
            offset = 0;
            for ( int i = 0; i < attrInfo2.size(); i++ ) {
                if ( attrInfo2[i].attrName == clause.right.col.indexName ) {
                    indexes2.push_back(i);
                    offsets2.push_back(offset);
                }
                offset += attrInfo2[i].attrLength;
            }
        } else {
            int offset = 0;
            for ( int i = 0; i < attrInfo1.size(); i++ ) {
                if ( attrInfo1[i].attrName == clause.right.col.indexName ) {
                    indexes1.push_back(i);
                    offsets1.push_back(offset);
                }
                offset += attrInfo1[i].attrLength;
            }
            
            offset = 0;
            for ( int i = 0; i < attrInfo2.size(); i++ ) {
                if ( attrInfo2[i].attrName == clause.left.col.indexName ) {
                    indexes2.push_back(i);
                    offsets2.push_back(offset);
                }
                offset += attrInfo2[i].attrLength;
            }
        }
    }
    
    int clauseIndex = -1;
    WhereClause finalClause;
    int point = -1; //估值, == 10, < or > 5
    for ( int i = 0; i < whereCommon.size(); i++ ) {
        if ( whereCommon[i].comOp == EQ_OP ) {
            finalClause = whereCommon[i];
            clauseIndex = i;
            point = 10;
        }
//        else if (whereCommon[i].comOp == GE_OP || whereCommon[i].comOp == GT_OP || whereCommon[i].comOp == LE_OP || whereCommon[i].comOp == LT_OP ) {
//            if (point < 5) {
//                point = 5;
//                clauseIndex = i;
//            }
//        }
    }
    
    if ( clauseIndex == -1 ) {
        for ( auto &record1: records1 ) {
            for ( auto &record2: records2 ) {
                bool satisfy = true;
                for ( int i = 0; i < indexes1.size(); i++ ) {
                    if ( whereCommon[i].left.col.tbName == tbList[0] ) {
                        if ( !TypeCompWithComOp(record1._data.data() + RECORD_HEAD * 4 + offsets1[i],
                                                record2._data.data() + RECORD_HEAD * 4 + offsets2[i],
                                                attrInfo1[indexes1[i]].attrType,
                                                whereCommon[i].comOp,
                                                attrInfo1[indexes1[i]].attrLength)) {
                            satisfy = false;
                            break;
                        }
                    } else {
                        if ( !TypeCompWithComOp(record2._data.data() + RECORD_HEAD * 4 + offsets2[i],
                                                record1._data.data() + RECORD_HEAD * 4 + offsets1[i],
                                                attrInfo1[indexes1[i]].attrType,
                                                whereCommon[i].comOp,
                                                attrInfo1[indexes1[i]].attrLength)) {
                            satisfy = false;
                            break;
                        }
                    }
                }
                if ( satisfy ) {
                    data.emplace_back();
                    for ( int i = 0; i < sindexes1.size(); i++ ) {
                        data[data.size() - 1].push_back(record1._data.substr(RECORD_HEAD * 4 + soffsets1[i],
                                                                             attrInfo1[sindexes1[i]].attrLength));
                    }
                    for ( int i = 0; i < sindexes2.size(); i++ ) {
                        data[data.size() - 1].push_back(record2._data.substr(RECORD_HEAD * 4 + soffsets2[i],
                                                                             attrInfo2[sindexes2[i]].attrLength));
                    }
                }
            }
        }
    } else {
        std::map<std::string, std::vector<int>> tbMap;
        for ( int i = 0; i < records2.size(); i++ ) {
            std::string tem = records2[i]._data.substr(RECORD_HEAD * 4 + offsets2[clauseIndex],
                                                       attrInfo2[indexes2[clauseIndex]].attrLength);
            auto find = tbMap.find(tem);
            if ( find == tbMap.end()) {
                tbMap.emplace(std::piecewise_construct,
                              std::forward_as_tuple(tem),
                              std::forward_as_tuple());
            }
            tbMap.at(tem).push_back(i);
        }
        
        for ( auto record: records1 ) {
            std::string tem = record._data.substr(RECORD_HEAD * 4 + offsets1[clauseIndex],
                                                  attrInfo1[indexes1[clauseIndex]].attrLength);
            auto find = tbMap.find(tem);
            if ( find == tbMap.end()) {
                continue;
            }
            
            for ( auto i: tbMap.at(tem)) {
                bool satisfy = true;
                for ( int j = 0; j < indexes1.size(); j++ ) {
                    if ( whereCommon[j].left.col.tbName == tbList[0] ) {
                        if ( !TypeCompWithComOp(record._data.data() + RECORD_HEAD * 4 + offsets1[j],
                                                records2[i]._data.data() + RECORD_HEAD * 4 + offsets2[j],
                                                attrInfo1[indexes1[j]].attrType,
                                                whereCommon[j].comOp,
                                                attrInfo1[indexes1[j]].attrLength)) {
                            satisfy = false;
                            break;
                        }
                    } else {
                        if ( !TypeCompWithComOp(records2[i]._data.data() + RECORD_HEAD * 4 + offsets2[j],
                                                record._data.data() + RECORD_HEAD * 4 + offsets1[j],
                                                attrInfo1[indexes1[j]].attrType,
                                                whereCommon[j].comOp,
                                                attrInfo1[indexes1[j]].attrLength)) {
                            satisfy = false;
                            break;
                        }
                    }
                }
                if ( satisfy ) {
                    data.emplace_back();
                    for ( int j = 0; j < sindexes1.size(); j++ ) {
                        data[data.size() - 1].push_back(record._data.substr(RECORD_HEAD * 4 + soffsets1[j],
                                                                            attrInfo1[sindexes1[j]].attrLength));
                    }
                    for ( int j = 0; j < sindexes2.size(); j++ ) {
                        data[data.size() - 1].push_back(records2[i]._data.substr(RECORD_HEAD * 4 + soffsets2[j],
                                                                                 attrInfo2[sindexes2[j]].attrLength));
                    }
                }
            }
        }
    }
    
    //print the result
    std::string splitLine;
    splitLine.assign(80, '=');
    printf("%s\n", splitLine.c_str());
    for ( auto &i : sindexes1 ) {
        printf("%s\t", attrInfo1[i].attrName.c_str());
    }
    
    for ( auto &i : sindexes2 ) {
        printf("%s\t", attrInfo2[i].attrName.c_str());
    }
    
    printf("\n");
    printf("%s\n", splitLine.c_str());
    
    for ( auto r: data ) {
        int i = 0;
        for ( i = 0; i < sindexes1.size(); i++ ) {
            if ( attrInfo1[sindexes1[i]].attrType == INT ) {
                int *t = (int *) (r[i].c_str());
                printf("%d\t", t[0]);
            } else if ( attrInfo1[sindexes1[i]].attrType == FLOAT ) {
                auto t = (float *) (r[i].c_str());
                printf("%.2f\t", t[0]);
            } else {
                printf("%s\t", r[i].c_str());
            }
        }
        for ( int j = 0; j < sindexes2.size(); j++, i++ ) {
            if ( attrInfo2[sindexes2[j]].attrType == INT ) {
                auto t = (int *) (r[i].c_str());
                printf("%d\t", t[0]);
            } else if ( attrInfo2[sindexes2[j]].attrType == FLOAT ) {
                auto t = (float *) (r[i].c_str());
                printf("%.2f\t", t[0]);
            } else {
                printf("%s\t", r[i].c_str());
            }
        }
        printf("\n");
    }
    
    printf("%s\n", splitLine.c_str());
    printf("select %lu lines\n", data.size());
}

bool DatabaseHandle::_checkInsert(const std::vector<AttrInfo> &info, const std::vector<DataAttr> &data) {
    static const int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (data.size() != info.size()) return false;
    for (int i = 0; i < data.size(); i++) {
        if (info[i].attrType == STRING) {
            if (data[i].data.size() > info[i].attrLength) return false;
            if (!(data[i].attrType == STRING)) return false;
        } else if (info[i].attrType == DATE) {
            if (data[i].data.size() != 10) return false;
            int year, month, day;
            if (sscanf(data[i].data.substr(0, 4).c_str(), "%d", &year) == 0) return false;
            if (sscanf(data[i].data.substr(5, 7).c_str(), "%d", &month) == 0) return false;
            if (sscanf(data[i].data.substr(8, 10).c_str(), "%d", &day) == 0) return false;
            if (year < 1000) return false;
            if (month < 0 || month > 12) return false;
            if (month == 2) {
                if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
                    if (day <= 0 || day > 29) {
                        return false;
                    }
                } else {
                    if (day <= 0 || day > 28) {
                        return false;
                    }
                }
            } else {
                if (day <= 0 || day > days[month - 1]) {
                    return false;
                }
            }
        } else if (info[i].attrType == INT){
            if (data[i].attrType != INT) return false;
        } else if (info[i].attrType == FLOAT) {
            if (data[i].attrType != INT && data[i].attrType != FLOAT) return false;
        }
    }
    return true;
}
