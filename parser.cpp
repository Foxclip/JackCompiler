#include <algorithm>
#include "parser.h"

void Parser::writeXML(std::string line) {
    std::ofstream stream(outputFilename, std::ios_base::app);
    stream << line.c_str() << std::endl;
}

std::string Parser::tokenName() {
    return tokenizer.currentToken().token;
}

TokenType Parser::tokenType() {
    return tokenizer.currentToken().type;
}

void Parser::eat(bool valid, std::string whatExpected) {
    if(tokenizer.hasMoreTokens()) {
        if(valid) {
            writeXML("<" + typeToStr(tokenType()) + "> " + tokenName() + " </" + typeToStr(tokenType()) + ">");
            tokenizer.advance();
        } else {
            throw SyntaxError(std::string("'" + tokenName() + "'" + ": " + whatExpected + " expected").c_str());
        }
    } else {
        throw SyntaxError(std::string(whatExpected + " expected, but file ended").c_str());
    }
}

void Parser::eatIdentifier() {
    eat(tokenType() == TT_IDENTIFIER, "identifier");
}

void Parser::eatStr(std::string str) {
    eat(str == tokenName(), str);
}

void Parser::parseClass() {
    writeXML("<class>");
    eatStr("class");
    eatIdentifier();
    eatStr("{");
    while(true) {
        try {
            parseClassVarDec();
        } catch(SyntaxError e) {
            break;
        }
    }
    while(true) {
        try {
            parseSubroutineDec();
        } catch(SyntaxError e) {
            break;
        }
    }
    eatStr("}");
    writeXML("</class>");
}

void Parser::parseClassVarDec() {
    writeXML("<classVarDec>");
    eat(tokenName() == "static" || tokenName() == "field", "'static' or 'field'");
    eatIdentifier();
    while(true) {
        try {
            eatStr(",");
            eatIdentifier();
        } catch(SyntaxError e) {
            break;
        }
    }
    eatStr(";");
    writeXML("</classVarDec>");
}

void Parser::parseSubroutineDec() {
    writeXML("<subroutineDec>");
    eat(tokenName() == "constructor" || tokenName() == "function" || tokenName() == "method", "'constructor', 'function' or 'method'");
    eat(tokenName() == "void" || tokenType() == TT_IDENTIFIER, "'void' or identifier");
    eatIdentifier();
    eatStr("(");
    parseParameterList();
    eatStr(")");
    parseSubroutineBody();
    writeXML("</subroutineDec>");
}

void Parser::parseParameterList() {
    throw NotImplementedError("Parameter list is not yet implemented");
}

void Parser::parseSubroutineBody() {
    throw NotImplementedError("Subroutine body is not yet implemented");
}

void Parser::parseStatements() {
    if(tokenName() == "let") {
        parseLetStatement();
    }
}

void Parser::parseLetStatement() {
}

void Parser::parse(std::string outputFilename, Tokenizer tokenizer) {
    std::cout << "Parsing " + outputFilename << std::endl;
    this->outputFilename = outputFilename;
    this->tokenizer = tokenizer;
    try {
        parseClass();
        debugPrintLine("Succesfully parsed", DL_PARSER);
    } catch(ParserError e) {
        std::cout << "Parser error: " + std::string(e.what()) << std::endl;
    }
}