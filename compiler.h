#pragma once

#include <functional>
#include "tokenizer.h"
#include "debug.h"

struct SymbolTableEntry {
    std::string name;
    std::string type;
    std::string kind;
    int index;
};

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

struct SemanticError : public CompileError {
public:
    SemanticError(std::string msg) {
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
    std::vector<SymbolTableEntry> classSymbolTable;
    int classFieldCount = 0;
    int classStaticCount = 0;
    int subroutineArgCount = 0;
    int subroutineLocalCount = 0;
    std::string subroutineKind;
    std::string subroutineName;
    std::string className;
    std::vector<SymbolTableEntry> subroutineSymbolTable;
    std::string outputXMLFilename;
    std::string outputVMFilename;
    double xmlIndentLevel = 0;
    int runningIndex = 0;

    void writeXML(std::string line);
    void writeVM(std::string line);
    std::string tokenName();
    int tokenType();
    std::string eat(bool valid, std::string whatExpected);
    std::string eatIdentifier();
    std::string eatType();
    std::string eatStr(std::string str);
    SymbolTableEntry findInSymbolTables(std::string name);
    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
    void addArgument();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileLetStatement();
    void compileIfStatement();
    void compileWhileStatement();
    void compileDoStatement();
    void compileReturnStatement();
    bool compileExpression();
    int compileExpressionList();
    void compileTerm();
    std::string compileOp();
    void compileSubroutineCall();

};
