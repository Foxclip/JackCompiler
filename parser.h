#pragma once

#include "tokenizer.h"
#include "debug.h"

class Parser {

public:

    void parse(std::string outputFilename, Tokenizer tokenizer);

private:

    Tokenizer tokenizer;
    std::string outputFilename;

    void writeXML(std::string line);
    void eat();
    void eat(std::string str);
    void parseClass();
    void parseClassVarDec();
    void parseSubroutineDec();
    void parseStatements();
    void parseLetStatement();

};