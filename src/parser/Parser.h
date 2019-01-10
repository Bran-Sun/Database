//
// Created by 孙桢波 on 2018-12-20.
//

#ifndef DATABASE_PARSER_H
#define DATABASE_PARSER_H

#include <vector>
#include "Lexer.h"
#include "../Action/Action.h"
#include "../smmanager/DataInfo.h"
#include "../Action/ShowAction.h"
#include "../Action/CreateAction.h"
#include "../Action/DropAction.h"
#include "../Action/UseAction.h"
#include "../Action/TableAction.h"

namespace parser
{
    class Parser
    {
    public:
        Parser() {}
    
        std::vector<std::shared_ptr<Action>> parse(const std::string &content);
        
    private:
        void _parseProgram();
        void _parseStmt();
        void _parseStmtList();
        
        void _parseShowStmt();
        void _parseLabel(TokenType tok);
        std::string _parseIdentifier();
        void _parseCreateStmt();
        void _parseDropStmt();
        void _parseUseStmt();
        void _parseTbStmt();
        
        void _parseFieldList();
        void _parseField();
        void _parseType(AttrInfo &info);
        bool _parseNullReceiver();
        void _parseFieldNext();
        
        std::string _parseVALUEDATA();
        int _convertToInt(std::string data);
        
        void _parseValueLists();
        void _parseValueNext();
        void _parseValueList();
        void _parseValue();
        void _parseValueReceiver();
        
        void _parseWhereClauseLists();
        void _parseWhereClause();
        void _parseWhereClauseReceiver();
        Col _parseCol();
        void _parseColSecond();
        WhereVal _parseExpr();
        
        void _parseSetClauseLists();
        void _parseSetClause();
        void _parseSetClauseReceiver();
        
        void _parseSelector();
        Col _parseSeCol();
        void _parseTableLists();
        
    private:
        std::vector<Col> _selector;
        bool _selectAll;
        std::vector<std::string> _tbList;
        
        std::vector<SetClause> _setClause;
        WhereClause _where;
        std::vector<std::shared_ptr<Action>> _actions;
        std::vector<AttrInfo> _attrInfo;
        std::vector<WhereClause> _whereClause;
        std::vector<std::vector<DataAttr>> _data;
        int _primaryNum;
        Lexer _lexer;
        Token _lookahead;
    };
}

#endif //DATABASE_PARSER_H
