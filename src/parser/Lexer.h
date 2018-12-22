//
// Created by 孙桢波 on 2018-12-20.
//

#ifndef DATABASE_LEXER_H
#define DATABASE_LEXER_H

#include <string>
#include <map>
#include <algorithm>

namespace parser
{
    enum class TokenType{
        EOS,
        
        DATABASE,
        DATABASES,
        TABLE,
        TABLES,
        SHOW,
        CREATE,
        DROP,
        USE,
        PRIMARY,
        KEY,
        NOT,
        NUL,
        INSERT,
        INTO,
        VALUES,
        DELETE,
        FROM,
        WHERE,
        UPDATE,
        SET,
        SELECT,
        IS,
        INT,
        VARCHAR,
        DESC,
        REFERENCES,
        INDEX,
        AND,
        DATE,
        FLOAT,
        FOREIGN,
        
        IDENTIFIER,
        VALUE_INT,
        VALUE_FLOAT,
        VALUE_STRING,
        
        COMMA,
        LEFTPARENTHESIS,
        RIGHTPARENTHESIS,
        SEMICOLON,
        PERIOD,
        STARKEY,
        
        EQ,
        NE,
        LT,
        GT,
        LE,
        GE,
        EQUAL,
      
        ERROR
    };
    
    
    
    struct Token{
        Token() {}
        Token(TokenType t): type(t) {}
        Token(TokenType t, const std::string &v): type(t), value(v) {}
        std::string value;
        TokenType type;
    };
    
    class Lexer
    {
    public:
        Lexer() {}
        Lexer(const std::string &text): _text(text) { _pos = 0; }
        Token next();
        void setText(const std::string &text) { _text = text; }
    public:
        static std::map<std::string, TokenType> keyword_map;
        
    private:
    
    private:
        std::string _text;
        std::string _content;
        int _pos;
    };
    
}


#endif //DATABASE_LEXER_H
