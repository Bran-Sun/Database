//
// Created by 孙桢波 on 2018-12-20.
//

#ifndef DATABASE_PARSER_H
#define DATABASE_PARSER_H

#include <vector>
#include "Lexer.h"
#include "../Action/Action.h"
#include "../smmanager/DataInfo.h"

namespace parser
{
    class Parser
    {
    public:
        Parser() {}
        
        void parse(std::string &content);
        
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
        
        std::vector<AttrInfo> _parseFieldList();
        AttrInfo _parseField();
    private:
        std::vector<Action> _actions;
        Lexer _lexer;
        Token _lookahead;
    };
}

#endif //DATABASE_PARSER_H
