#include <utility>

#include <utility>

//
// Created by 孙桢波 on 2018-12-20.
//

#ifndef DATABASE_ERROR_H
#define DATABASE_ERROR_H

#include <string>

class Error : public std::exception
{
public:
    enum ErrorType {
        INSERT_ERROR,
        DELETE_ERROR,
    };
    
    Error(std::string errorMsg, ErrorType type): _errorMsg(std::move(errorMsg)), _type(type) { }
    const char * what() const noexcept override {
        return _errorMsg.c_str();
    }
    
    ErrorType getErrorType() const {
        return _type;
    }
    
private:
    std::string _errorMsg;
    ErrorType _type;
};


#endif //DATABASE_ERROR_H
