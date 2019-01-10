//
// Created by 孙桢波 on 2018-12-20.
//

#include <sstream>
#include "Parser.h"
#include "../smmanager/DataInfo.h"

namespace parser {
    std::vector<std::shared_ptr<Action>> Parser::parse(const std::string &content) {
        _lexer.setText(content);
        _actions.clear();
        _lookahead = _lexer.next();
        
        _parseProgram();
        
        return _actions;
    }
    
    void Parser::_parseProgram() {
        if (_lookahead.type == TokenType::EOS) {
            return;
        }
        _parseStmt();
        _parseStmtList();
    }
    
    void Parser::_parseStmt() {
        if (_lookahead.type == TokenType::SHOW) {
            _parseShowStmt();
            _parseLabel(TokenType::SEMICOLON);
        } else if (_lookahead.type == TokenType::CREATE) {
            _parseCreateStmt();
            _parseLabel(TokenType::SEMICOLON);
        } else if (_lookahead.type == TokenType::DROP) {
            _parseDropStmt();
            _parseLabel(TokenType::SEMICOLON);
        } else if (_lookahead.type == TokenType::USE) {
            _parseUseStmt();
            _parseLabel(TokenType::SEMICOLON);
        } else if (_lookahead.type == TokenType::DESC || _lookahead.type == TokenType::INSERT ||
                    _lookahead.type == TokenType::DELETE || _lookahead.type == TokenType::UPDATE ||
                    _lookahead.type == TokenType::SELECT) {
            _parseTbStmt();
            _parseLabel(TokenType::SEMICOLON);
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseShowStmt() {
        _parseLabel(TokenType::SHOW);
        if (_lookahead.type == TokenType::DATABASES) {
            //TODO
            _parseLabel(TokenType::DATABASES);
            _actions.push_back(std::make_shared<ShowDatabase>(ShowDatabase()));
        } else if (_lookahead.type == TokenType::TABLES) {
            //TODO
            _parseLabel(TokenType::TABLES);
            _actions.push_back(std::make_shared<ShowTables>(ShowTables()));
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseCreateStmt()
    {
        _parseLabel(TokenType::CREATE);
        if (_lookahead.type == TokenType::DATABASE) {
            _parseLabel(TokenType::DATABASE);
            std::string baseName = _parseIdentifier();
            //TODO
            _actions.push_back(std::make_shared<CreateDatabase>(baseName));
        } else if (_lookahead.type == TokenType::TABLE) {
            _parseLabel(TokenType::TABLE);
            std::string tableName = _parseIdentifier();
            _parseLabel(TokenType::LEFTPARENTHESIS);
            _parseFieldList();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            //TODO
            _actions.push_back(std::make_shared<CreateTable>(tableName, _attrInfo));
        } else if (_lookahead.type == TokenType::INDEX) {
            _parseLabel(TokenType::INDEX);
            std::string tableName = _parseIdentifier();
            _parseLabel(TokenType::LEFTPARENTHESIS);
            std::string indexName = _parseIdentifier();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            //TODO
            _actions.push_back(std::make_shared<CreateIndex>(tableName, indexName));
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }

    void Parser::_parseDropStmt()
    {
        _parseLabel(TokenType::DROP);
        if (_lookahead.type == TokenType::DATABASE) {
            _parseLabel(TokenType::DATABASE);
            std::string baseName = _parseIdentifier();
            //TODO
            _actions.push_back(std::make_shared<DropDatabase>(baseName));
        } else if (_lookahead.type == TokenType::TABLE) {
            _parseLabel(TokenType::TABLE);
            std::string tableName = _parseIdentifier();
            _actions.push_back(std::make_shared<DropTable>(tableName));
        } else if (_lookahead.type == TokenType::INDEX) {
            _parseLabel(TokenType::TABLE);
            std::string tableName = _parseIdentifier();
            _parseLabel(TokenType::LEFTPARENTHESIS);
            std::string indexName = _parseIdentifier();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            //TODO
            _actions.push_back(std::make_shared<DropIndex>(tableName, indexName));
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseUseStmt()
    {
        _parseLabel(TokenType::USE);
        std::string baseName = _parseIdentifier();
        //TODO
        _actions.emplace_back(std::make_shared<UseDatabase>(baseName));
    }
    
    void Parser::_parseTbStmt()
    {
        if (_lookahead.type == TokenType::DESC) {
            _parseLabel(TokenType::DESC);
            std::string tbName = _parseIdentifier();
            //TODO
            _actions.push_back(std::make_shared<DescTable>(tbName));
        } else if (_lookahead.type == TokenType::INSERT){
            _parseLabel(TokenType::INSERT);
            _parseLabel(TokenType::INTO);
            std::string tbName = _parseIdentifier();
            _parseLabel(TokenType::VALUES);
            _parseValueLists();
            //TODO
            _actions.push_back(std::make_shared<InsertTable>(tbName, _data));
        } else if (_lookahead.type == TokenType::DELETE) {
            _whereClause.clear();
            _parseLabel(TokenType::DELETE);
            _parseLabel(TokenType::FROM);
            std::string tbName = _parseIdentifier();
            _parseLabel(TokenType::WHERE);
            _parseWhereClauseLists();
            //TODO
            _actions.push_back(std::make_shared<DeleteTable>(tbName, _whereClause));
        } else if (_lookahead.type == TokenType::UPDATE) {
            _setClause.clear();
            _whereClause.clear();
            _parseLabel(TokenType::UPDATE);
            std::string tbName = _parseIdentifier();
            _parseLabel(TokenType::SET);
            _parseSetClauseLists();
            _parseLabel(TokenType::WHERE);
            _parseWhereClauseLists();
            //TODO
            _actions.push_back(std::make_shared<UpdateTable>(tbName, _whereClause, _setClause));
        } else if (_lookahead.type == TokenType::SELECT) {
            _selectAll = false;
            _selector.clear();
            _tbList.clear();
            _whereClause.clear();
            _parseLabel(TokenType::SELECT);
            _parseSelector();
            _parseLabel(TokenType::FROM);
            _parseTableLists();
            _parseLabel(TokenType::WHERE);
            _parseWhereClauseLists();
            //TODO
            _actions.push_back(std::make_shared<SelectTable>(_tbList, _selector, _selectAll, _whereClause));
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseLabel(TokenType tok) {
        if (_lookahead.type == tok) {
            _lookahead = _lexer.next();
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseStmtList() {
        if (_lookahead.type == TokenType::EOS) {
            return;
        }
        _parseStmt();
        _parseStmtList();
    }
    
    void Parser::_parseFieldList() {
        _attrInfo.clear();
        _primaryNum = 0;
        if (_lookahead.type == TokenType::IDENTIFIER || _lookahead.type == TokenType::PRIMARY || _lookahead.type == TokenType::FOREIGN) {
            _parseField();
            _parseFieldNext();
            
            if (_primaryNum != 1) {
                //TODO
                printf("need to have one primary key!\n");
            }
        } else if (_lookahead.type == TokenType::RIGHTPARENTHESIS){
            return;
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseFieldNext()
    {
        if (_lookahead.type == TokenType::COMMA) {
            _parseLabel(TokenType::COMMA);
            _parseField();
            _parseFieldNext();
        } else if (_lookahead.type == TokenType::RIGHTPARENTHESIS) {
            return;
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseField() {
        AttrInfo info;
        if (_lookahead.type == TokenType::IDENTIFIER) {
            info.attrName = _parseIdentifier();
            _parseType(info);
            info.isNull = _parseNullReceiver();
            _attrInfo.push_back(info);
        } else if (_lookahead.type == TokenType::PRIMARY) {
            _parseLabel(TokenType::PRIMARY);
            _parseLabel(TokenType::KEY);
            _parseLabel(TokenType::LEFTPARENTHESIS);
            std::string indexName = _parseIdentifier();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            
            for (auto &item: _attrInfo) {
                if (item.attrName == indexName) {
                    if (!item.isPrimary)
                    {
                        item.isPrimary = true;
                        _primaryNum++;
                    }
                }
            }
        } else if (_lookahead.type == TokenType::FOREIGN) {
            _parseLabel(TokenType::FOREIGN);
            _parseLabel(TokenType::KEY);
            _parseLabel(TokenType::LEFTPARENTHESIS);
            std::string indexName = _parseIdentifier();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            _parseLabel(TokenType::REFERENCES);
            std::string tbName = _parseIdentifier();
            _parseLabel(TokenType::LEFTPARENTHESIS);
            std::string forName = _parseIdentifier();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            
            for (auto &item: _attrInfo) {
                if (item.attrName == indexName) {
                    item.isForeign = true;
                    item.foreignTb = tbName;
                    item.foreignIndex = forName;
                }
            }
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    bool Parser::_parseNullReceiver()
    {
        if (_lookahead.type == TokenType::NOT) {
            _parseLabel(TokenType::NOT);
            _parseLabel(TokenType::NUL);
            return false;
        } else if (_lookahead.type == TokenType::RIGHTPARENTHESIS || _lookahead.type == TokenType::COMMA) {
            return true;
        } else {
            //TODO
            return false;
        }
    }
    
    void Parser::_parseType(AttrInfo &info) {
        if (_lookahead.type == TokenType::INT) {
            _parseLabel(TokenType::INT);
            _parseLabel(TokenType::LEFTPARENTHESIS);
            info.attrLength = _convertToInt(_parseVALUEDATA());
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            info.attrType = INT;
        } else if (_lookahead.type == TokenType::CHAR) {
            _parseLabel(TokenType::CHAR);
            _parseLabel(TokenType::LEFTPARENTHESIS);
            info.attrLength = _convertToInt(_parseVALUEDATA());
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            info.attrType = STRING;
        } else if (_lookahead.type == TokenType::DATE) {
            _parseLabel(TokenType::DATE);
            info.attrType = DATE;
            info.attrLength = 10;
        } else if (_lookahead.type == TokenType::FLOAT) {
            _parseLabel(TokenType::FLOAT);
            info.attrLength = 4;
            info.attrType = FLOAT;
        } else {
            //TODO
        }
    }
    
    std::string Parser::_parseIdentifier() {
        if (_lookahead.type == TokenType::IDENTIFIER) {
            std::string tem = _lookahead.value;
            _lookahead = _lexer.next();
            return tem;
        } else {
            //TODO
            return "";
        }
    }
    
    std::string Parser::_parseVALUEDATA()
    {
        if (_lookahead.type == TokenType::VALUE_INT) {
            std::stringstream sin(_lookahead.value);
            int data;
            sin >> data;
            _lookahead = _lexer.next();
            return std::string((char*)&data, 4);
        } else if (_lookahead.type == TokenType::VALUE_FLOAT) {
            std::stringstream sin(_lookahead.value);
            float data;
            sin >> data;
            _lookahead = _lexer.next();
            return std::string((char*)&data, 4);
        } else if (_lookahead.type == TokenType::VALUE_STRING || _lookahead.type == TokenType::NUL) {
            std::string s = _lookahead.value;
            _lookahead = _lexer.next();
            return s;
        } else {
            //TODO
            return std::string("");
        }
    }
    
    void Parser::_parseValueLists()
    {
        _data.clear();
        _parseLabel(TokenType::LEFTPARENTHESIS);
        _parseValueList();
        _parseLabel(TokenType::RIGHTPARENTHESIS);
        _parseValueNext();
    }
    
    void Parser::_parseValueNext()
    {
        if (_lookahead.type == TokenType::COMMA) {
            _parseLabel(TokenType::COMMA);
            _parseLabel(TokenType::LEFTPARENTHESIS);
            _parseValueList();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            _parseValueNext();
        } else if (_lookahead.type == TokenType::SEMICOLON) {
            return;
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseValueList() {
        _data.emplace_back(); //add a new record
        
        _parseValue();
        _parseValueReceiver();
    }
    
    void Parser::_parseValue() {
        if (_lookahead.type == TokenType::VALUE_INT) {
            std::string d = _parseVALUEDATA();
            _data[_data.size() - 1].emplace_back(d, INT);
        } else if (_lookahead.type == TokenType::VALUE_FLOAT) {
            std::string d = _parseVALUEDATA();
            _data[_data.size() - 1].emplace_back(d, FLOAT);
        } else if (_lookahead.type == TokenType::VALUE_STRING) {
            std::string d = _parseVALUEDATA();
            _data[_data.size() - 1].emplace_back(d, STRING);
        } else if (_lookahead.type == TokenType::NUL) {
            _parseVALUEDATA();
            _data[_data.size() - 1].emplace_back(true);
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseValueReceiver()
    {
        if (_lookahead.type == TokenType::COMMA) {
            _parseLabel(TokenType::COMMA);
            _parseValue();
            _parseValueReceiver();
        } else if (_lookahead.type == TokenType::RIGHTPARENTHESIS) {
            return;
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    int Parser::_convertToInt(std::string data)
    {
        return ((int*)data.data())[0];
    }
    
    void Parser::_parseWhereClauseLists()
    {
        _parseWhereClause();
        _parseWhereClauseReceiver();
    }
    
    void Parser::_parseWhereClauseReceiver()
    {
        while (true)
        {
            if ( _lookahead.type == TokenType::AND )
            {
                _parseLabel(TokenType::AND);
                _parseWhereClause();
            } else if ( _lookahead.type == TokenType::SEMICOLON )
            {
                return;
            } else
            {
                throw Error("parse error.\n", Error::PARSER_ERROR);
            }
        }
    }
    
    void Parser::_parseWhereClause() {
        _where.left.isVal = false;
        _where.left.col = _parseCol();
        _parseColSecond();
        _whereClause.push_back(_where);
    };
    
    Col Parser::_parseCol() {
        Col ans;
        
        std::string name = _parseIdentifier();
        if (_lookahead.type == TokenType::PERIOD) {
            _parseLabel(TokenType::PERIOD);
            std::string indexName = _parseIdentifier();
            ans.tbName = name;
            ans.indexName = indexName;
        } else {
            ans.tbName = "";
            ans.indexName = name;
        }
        return ans;
    }
    
    void Parser::_parseColSecond() {
        if (_lookahead.type ==  TokenType::EQ) {
            _parseLabel(TokenType::EQ);
            _where.comOp = EQ_OP;
            _where.right = _parseExpr();
        } else if (_lookahead.type == TokenType::NE) {
            _parseLabel(TokenType::NE);
            _where.comOp = NE_OP;
            _where.right = _parseExpr();
        } else if (_lookahead.type == TokenType::GE) {
            _parseLabel(TokenType::GE);
            _where.comOp = GE_OP;
            _where.right = _parseExpr();
        } else if (_lookahead.type == TokenType::LE) {
            _parseLabel(TokenType::LE);
            _where.comOp = LE_OP;
            _where.right = _parseExpr();
        } else if (_lookahead.type == TokenType::GT) {
            _parseLabel(TokenType::GT);
            _where.comOp = GT_OP;
            _where.right = _parseExpr();
        } else if (_lookahead.type == TokenType::LT) {
            _parseLabel(TokenType::LT);
            _where.comOp = LT_OP;
            _where.right = _parseExpr();
        } else if (_lookahead.type == TokenType::IS) {
            _parseLabel(TokenType::IS);
            if (_lookahead.type == TokenType::NOT) {
                _parseLabel(TokenType::NOT);
                _parseLabel(TokenType::NUL);
                _where.right.isVal = true;
                _where.right.useNull = true;
                _where.right.isNull = true;
            } else {
                _parseLabel(TokenType::NUL);
                _where.right.isVal = true;
                _where.right.useNull = true;
                _where.right.isNull = false;
            }
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    WhereVal Parser::_parseExpr() {
        WhereVal ans;
        if (_lookahead.type == TokenType::VALUE_INT) {
            std::string d = _parseVALUEDATA();
            ans.isVal = true;
            ans.useNull = false;
            ans.value = d;
            ans.type = INT;
        } else if (_lookahead.type == TokenType::VALUE_FLOAT) {
            std::string d = _parseVALUEDATA();
            ans.isVal = true;
            ans.useNull = false;
            ans.value = d;
            ans.type = FLOAT;
        } else if (_lookahead.type == TokenType::VALUE_STRING) {
            std::string d = _parseVALUEDATA();
            ans.isVal = true;
            ans.useNull = false;
            ans.value = d;
            ans.type = STRING;
        } else if (_lookahead.type == TokenType::NUL) {
            std::string d = _parseVALUEDATA();
            ans.isVal = true;
            ans.useNull = false;
            ans.value = d;
            ans.type = NUL;
        } else if (_lookahead.type == TokenType::IDENTIFIER) {
            ans.col = _parseCol();
            ans.isVal = false;
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
        return ans;
    }
    
    void Parser::_parseSetClauseLists()
    {
        _parseSetClause();
        _parseSetClauseReceiver();
    }
    
    void Parser::_parseSetClause() {
        std::string name = _parseIdentifier();
        _parseLabel(TokenType::EQ);
        if (_lookahead.type == TokenType::VALUE_INT) {
            std::string d = _parseVALUEDATA();
            _setClause.emplace_back(name, d, INT);
        } else if (_lookahead.type == TokenType::VALUE_FLOAT) {
            std::string d = _parseVALUEDATA();
            _setClause.emplace_back(name, d, FLOAT);
        } else if (_lookahead.type == TokenType::VALUE_STRING) {
            std::string d = _parseVALUEDATA();
            _setClause.emplace_back(name, d, STRING);
        } else if (_lookahead.type == TokenType::NUL) {
            std::string d = _parseVALUEDATA();
            _setClause.emplace_back(name, d, NUL);
        }
    }
    
    void Parser::_parseSetClauseReceiver()
    {
        if (_lookahead.type == TokenType::COMMA) {
            _parseLabel(TokenType::COMMA);
            _parseSetClause();
            _parseSetClauseReceiver();
        } else if (_lookahead.type == TokenType::WHERE) {
            return;
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    Col Parser::_parseSeCol() {
        if (_lookahead.type == TokenType::IDENTIFIER)  {
            return _parseCol();
        } else if (_lookahead.type == TokenType::AVG) {
            _parseLabel(TokenType::AVG);
            _parseLabel(TokenType::LEFTPARENTHESIS);
            Col col = _parseCol();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            col.isAgg = true;
            col.aggType = AVG;
            return col;
        } else if (_lookahead.type == TokenType::SUM) {
            _parseLabel(TokenType::SUM);
            _parseLabel(TokenType::LEFTPARENTHESIS);
            Col col = _parseCol();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            col.isAgg = true;
            col.aggType = SUM;
            return col;
        } else if (_lookahead.type == TokenType::MIN) {
            _parseLabel(TokenType::MIN);
            _parseLabel(TokenType::LEFTPARENTHESIS);
            Col col = _parseCol();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            col.isAgg = true;
            col.aggType = MIN;
            return col;
        } else if (_lookahead.type == TokenType::MAX) {
            _parseLabel(TokenType::MAX);
            _parseLabel(TokenType::LEFTPARENTHESIS);
            Col col = _parseCol();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            col.isAgg = true;
            col.aggType = MAX;
            return col;
        } else if (_lookahead.type == TokenType::COUNT) {
            _parseLabel(TokenType::COUNT);
            _parseLabel(TokenType::LEFTPARENTHESIS);
            Col col = _parseCol();
            _parseLabel(TokenType::RIGHTPARENTHESIS);
            col.isAgg = true;
            col.aggType = COUNT;
            //printf("fuck avg!\n");
            return col;
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseSelector() {
        if (_lookahead.type == TokenType::STARKEY) {
            _parseLabel(TokenType::STARKEY);
            _selectAll = true;
        } else if (_lookahead.type == TokenType::IDENTIFIER || _lookahead.type == TokenType::SUM || _lookahead.type == TokenType::MIN || _lookahead.type == TokenType::MAX || _lookahead.type == TokenType::AVG || _lookahead.type == TokenType::COUNT) {
            _selectAll = false;
            
            Col col = _parseSeCol();
            _selector.push_back(col);
            
            while (_lookahead.type == TokenType::COMMA) {
                _parseLabel(TokenType::COMMA);
                col = _parseSeCol();
                _selector.push_back(col);
            }
            
            if (_lookahead.type == TokenType::FROM) {
                return;
            } else {
                //TODO
            }
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
    void Parser::_parseTableLists() {
        _tbList.push_back(_parseIdentifier());
        while (_lookahead.type == TokenType::COMMA) {
            _parseLabel(TokenType::COMMA);
            _tbList.push_back(_parseIdentifier());
        }
        if (_lookahead.type == TokenType::WHERE) {
            return;
        } else {
            throw Error("parse error.\n", Error::PARSER_ERROR);
        }
    }
    
}