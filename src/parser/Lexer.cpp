//
// Created by 孙桢波 on 2018-12-20.
//

#include "Lexer.h"

namespace parser
{
    std::map<std::string, TokenType> Lexer::keyword_map{
            {"DATABASE",   TokenType::DATABASE},
            {"DATABASES",  TokenType::DATABASES},
            {"TABLE",      TokenType::TABLE},
            {"TABLES",     TokenType::TABLES},
            {"SHOW",       TokenType::SHOW},
            {"CREATE",     TokenType::CREATE},
            {"DROP",       TokenType::DROP},
            {"USE",        TokenType::USE},
            {"PRIMARY",    TokenType::PRIMARY},
            {"KEY",        TokenType::KEY},
            {"NOT",        TokenType::NOT},
            {"NULL",       TokenType::NUL},
            {"INSERT",     TokenType::INSERT},
            {"INTO",       TokenType::INTO},
            {"VALUES",     TokenType::VALUES},
            {"DELETE",     TokenType::DELETE},
            {"FROM",       TokenType::FROM},
            {"WHERE",      TokenType::WHERE},
            {"UPDATE",     TokenType::UPDATE},
            {"SET",        TokenType::SET},
            {"SELECT",     TokenType::SELECT},
            {"IS",         TokenType::IS},
            {"INT",        TokenType::INT},
            {"CHAR",       TokenType::CHAR},
            {"DESC",       TokenType::DESC},
            {"REFERENCES", TokenType::REFERENCES},
            {"INDEX",      TokenType::INDEX},
            {"AND",        TokenType::AND},
            {"DATE",       TokenType::DATE},
            {"FLOAT",      TokenType::FLOAT},
            {"FOREIGN",    TokenType::FOREIGN}
    };
    
    Token Lexer::next()
    {
        //pad
        _content.clear();
        
        while (( _pos < _text.length()) && (isspace(_text[_pos]) || (_text[_pos] == '\n'))) {
            _pos++;
        }
        if (_pos == _text.length()) return Token(TokenType::EOS);
        
        //number
        if (isdigit(_text[_pos]) || (_text[_pos] == '-')) {
            _content += _text[_pos];
            _pos++;
            while(isdigit(_text[_pos])) {
                _content += _text[_pos];
                _pos++;
            }
            
            if (_text[_pos] != '.') { //intnum
                return Token(TokenType::VALUE_INT, _content);
            }
            _content += _text[_pos];
            _pos++;
            while (isdigit(_text[_pos])) {
                _content += _text[_pos];
                _pos++;
            }
            
            return Token(TokenType::VALUE_FLOAT, _content);
        }
        
        //keyword or identifier
        if (isalpha(_text[_pos])) {
            while (isalnum(_text[_pos]) || (_text[_pos] == '_')) {
                _content += _text[_pos];
                _pos++;
            }
            
            std::string identifier = _content;
            //std::transform(identifier.begin(), identifier.end(), identifier.begin(), toupper);
            if (keyword_map.find(identifier) != keyword_map.end()) {
                return Token(keyword_map.at(identifier));
            } else {
                return Token(TokenType::IDENTIFIER, _content);
            }
        }
        
        //another identifier
        if (_text[_pos] == '`') {
            _pos++;
            if (!isalnum(_text[_pos])) {
                printf("lex error!\n");
                return Token(TokenType::ERROR);
            }
            
            while (isalnum(_text[_pos]) || (_text[_pos] == '_')) {
                _content += _text[_pos];
                _pos++;
            }
            
            if (_text[_pos] == '`') {
                _pos++;
                return Token(TokenType::IDENTIFIER, _content);
            } else {
                printf("lex error\n");
                return Token(TokenType::ERROR);
            }
        }
        
        //VALUE_STRING
        if (_text[_pos] == '\'') {
            _pos++;
            while (_pos < _text.length() && _text[_pos] != '\'') {
                if (_text[_pos] == '\\') {
                    if (_text[_pos + 1] == '\'') {
                        _pos = _pos + 2;
                        continue;
                    }
                }
                _content += _text[_pos];
                _pos++;
            }
            
            if (_pos == _text.length()) {
                printf("lex error\n");
                return Token(TokenType::ERROR);
            }
            
            _pos++;
            return Token(TokenType::VALUE_STRING, _content);
        }
        
        //other symbols
        if (_text[_pos] == ',') {
            _pos++;
            return Token(TokenType::COMMA);
        }
        
        if (_text[_pos] == '(') {
            _pos++;
            return Token(TokenType::LEFTPARENTHESIS);
        }
        
        if (_text[_pos] == ')') {
            _pos++;
            return Token(TokenType::RIGHTPARENTHESIS);
        }
        
        if (_text[_pos] == '.') {
            _pos++;
            return Token(TokenType::PERIOD);
        }
        
        if (_text[_pos] == ';') {
            _pos++;
            return Token(TokenType::SEMICOLON);
        }
        
        if (_text[_pos] == '*') {
            _pos++;
            return Token(TokenType::STARKEY);
        }
        
        if (_text[_pos] == '=') {
            _pos++;
            return Token(TokenType::EQ);
        }
        
        if (_text[_pos] == '<') {
            if (_text[_pos + 1] == '>') {
                _pos = _pos + 2;
                return Token(TokenType::NE);
            } else if (_text[_pos + 1] == '=') {
                _pos = _pos + 2;
                return Token(TokenType::LE);
            } else {
                _pos++;
                return Token(TokenType::LT);
            }
        }
        
        if (_text[_pos] == '>') {
            if (_text[_pos + 1] == '=') {
                _pos = _pos + 2;
                return Token(TokenType::GE);
            } else {
                _pos++;
                return Token(TokenType::GT);
            }
        }
        
        throw Error("lexer error!\n", Error::LEXER_ERROR);
        return Token(TokenType::EOS);
    }
}