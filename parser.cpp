#include "parser.h"

void Parser::writeXML(std::string line) {
    std::ofstream stream(outputFilename, std::ios_base::app);
    stream << line.c_str() << std::endl;
}

void Parser::eat() {
    if(tokenizer.hasMoreTokens()) {
        if(tokenizer.getToken().type == TT_ID) {
            writeXML("<identifier> " + tokenizer.getToken().token + " </identifier>");
            tokenizer.advance();
        } else {
            throw std::exception(std::string("identifier expected").c_str());
        }
    } else {
        throw std::exception(std::string("identifier expected, but file ended").c_str());
    }
}

void Parser::eat(std::string str) {
    if(tokenizer.hasMoreTokens()) {
        writeXML("<" + typeToStr(tokenizer.getToken().type) + "> " + str + " </" + typeToStr(tokenizer.getToken().type) + ">");
        if(str == tokenizer.getToken().token) {
            tokenizer.advance();
        } else {
            throw std::exception(std::string("'" + tokenizer.getToken().token + "'" + ": " + str + " expected").c_str());
        }
    } else {
        throw std::exception(std::string("'" + str + "'" + " expected, but file ended").c_str());
    }
}

void Parser::parseClass() {
    writeXML("<class>");
    eat("class");
    eat();
    eat("{");
    while(true) {
        try {
            parseClassVarDec();
        } catch(std::exception e) {
            break;
        }
    }
    while(true) {
        try {
            parseSubroutineDec();
        } catch(std::exception e) {
            break;
        }
    }
    eat("}");
    writeXML("</class>");
}

void Parser::parseClassVarDec() {
    throw std::exception("");
}

void Parser::parseSubroutineDec() {
    throw std::exception("");
}

void Parser::parseStatements() {
    if(tokenizer.getToken().token == "let") {
        parseLetStatement();
    }
}

void Parser::parseLetStatement() {
}

void Parser::parse(std::string outputFilename, Tokenizer tokenizer) {
    this->outputFilename = outputFilename;
    this->tokenizer = tokenizer;
    try {
        parseClass();
    } catch(std::exception e) {
        std::cout << "Parse error: " + std::string(e.what()) << std::endl;
    }
}