#pragma once

#include <functional>
#include "tokenizer.h"
#include "debug.h"

struct ParserError : public std::exception {

public:
    ParserError() {}
    ParserError(std::string msg): message(msg) {}
    const char *what() const throw() {
        return message.c_str();
    }

protected:
    std::string message;

};

struct SyntaxError : public ParserError {
public:
    SyntaxError(std::string msg) {
        message = msg;
    }
};

struct NotImplementedError : public ParserError {
public:
    NotImplementedError(std::string msg) {
        message = msg;
    }
};

class Parser {

public:
    void parse(std::string outputFilename, Tokenizer tokenizer);

private:
    Tokenizer tokenizer;
    std::string outputFilename;

    void writeXML(std::string line);
    std::string tokenName();
    TokenType tokenType();
    void eat(bool valid, std::string whatExpected);
    void eatIdentifier();
    void eatType();
    void eatStr(std::string str);
    void parseClass();
    void parseClassVarDec();
    void parseSubroutineDec();
    void parseParameterList();
    void parseSubroutineBody();
    void parseVarDec();
    void parseStatements();
    void parseLetStatement();
    void parseIfStatement();
    void parsewhileStatement();
    void parseDoStatement();
    void parseReturnStatement();
      

};