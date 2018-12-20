//
// Created by 孙桢波 on 2018-12-20.
//

#include <cstdio>
#include <iostream>
#include "Lexer.h"

int main() {
    using namespace parser;
    Lexer lex("database");
    Token token = lex.next();
    if (token.type == TokenType::DATABASE) {
        printf("success!\n");
    }
    
    std::string dd("instre");
    Lexer lex2("databases");
    Token token1 = lex2.next();
    printf("hello world!\n");
}
