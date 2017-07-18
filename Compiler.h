#pragma once

#include <functional>
#include "tokenizer.h"
#include "debug.h"

struct CompileError : public std::exception {

public:
    CompileError() {}
    CompileError(std::string msg): message(msg) {}
    const char *what() const throw() {
        return message.c_str();
    }

protected:
    std::string message;

};

struct SyntaxError : public CompileError {
public:
    SyntaxError(std::string msg) {
        message = msg;
    }
};

struct NotImplementedError : public CompileError {
public:
    NotImplementedError(std::string msg) {
        message = msg;
    }
};

class Compiler {

public:
    void compile(std::string outputFilename);

private:
    Tokenizer tokenizer;
    std::string outputFilename;
    double xmlIndentLevel = 0;

    void writeXML(std::string line);
    std::string tokenName();
    int tokenType();
    void eat(bool valid, std::string whatExpected);
    void eatIdentifier();
    void eatType();
    void eatStr(std::string str);
    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileLetStatement();
    void compileIfStatement();
    void compileWhileStatement();
    void compileDoStatement();
    void compileReturnStatement();
    void compileExpression();
    void compileExpressionList();
    void compileTerm();
    void compileOp();
    void compileSubroutineCall();

};