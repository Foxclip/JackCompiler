#include <algorithm>
#include "parser.h"

double xmlIndentLevel = 0;

void Parser::writeXML(std::string line) {
    int braceCount = 0;
    bool indentDone = false;
    for(char c: line) {
        if(c == '<' || c == '>') {
            braceCount++;
        }
    }
    if(braceCount == 2) {
        for(char c: line) {
            if(c == '/') {
                xmlIndentLevel -= 1;
                indentDone = true;
                break;
            }
        }
    }
    std::ofstream stream(outputFilename, std::ios_base::app);
    for(int i = 0; i < xmlIndentLevel; i++) {
        for(int j = 0; j < 2; j++) {
            stream << ' ';
        }
    }
    stream << line << std::endl;
    if(!indentDone) {
        for(char c : line) {
            if(c == '<' || c == '>') {
                xmlIndentLevel += 0.5;
            }
            if(c == '/') {
                xmlIndentLevel -= 2;
            }
        }
    }
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

void Parser::eatType() {
    eat(tokenName() == "int" || tokenName() == "char" || tokenName() == "boolean" || tokenType() == TT_IDENTIFIER, "type");
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
    if(tokenName() == "static" || tokenName() == "field") {
        writeXML("<classVarDec>");
    }
    eat(tokenName() == "static" || tokenName() == "field", "'static' or 'field'");
    eatType();
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
    if(tokenName() == "constructor" || tokenName() == "function" || tokenName() == "method") {
        writeXML("<subroutineDec>");
    }
    eat(tokenName() == "constructor" || tokenName() == "function" || tokenName() == "method", "'constructor', 'function' or 'method'");
    eat(tokenName() == "void" || tokenName() == "int" || tokenName() == "char" || tokenName() == "boolean" || tokenType() == TT_IDENTIFIER, "'void' or type");
    eatIdentifier();
    eatStr("(");
    parseParameterList();
    eatStr(")");
    parseSubroutineBody();
    writeXML("</subroutineDec>");
}

void Parser::parseParameterList() {
    writeXML("<parameterList>");
    try {
        eatType();
        eatIdentifier();
        while(true) {
            try {
                eatStr(",");
                eatType();
                eatIdentifier();
            } catch(SyntaxError e) {
                break;
            }
        }
    } catch(SyntaxError e) {}
    writeXML("</parameterList>");
}

void Parser::parseSubroutineBody() {
    writeXML("<subroutineBody>");
    eatStr("{");
    while(true) {
        try {
            parseVarDec();
        } catch(SyntaxError e) {
            break;
        }
    }
    parseStatements();
    eatStr("}");
    writeXML("</subroutineBody>");
}

void Parser::parseVarDec() {
    if(tokenName() == "var") {
        writeXML("<varDec>");
    }
    eatStr("var");
    eatType();
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
    writeXML("</varDec>");
}

void Parser::parseStatements() {
    throw NotImplementedError("...");
    while(true) {
        try {
            if(tokenName() == "let") {
                parseLetStatement();
            }
            if(tokenName() == "if") {
                parseIfStatement();
            }
            if(tokenName() == "while") {
                parsewhileStatement();
            }
            if(tokenName() == "do") {
                parseDoStatement();
            }
            if(tokenName() == "return") {
                parseReturnStatement();
            }
        } catch(SyntaxError e) {
            break;
        }
    }
}

void Parser::parseLetStatement() {
}

void Parser::parseIfStatement() {
}

void Parser::parsewhileStatement() {
}

void Parser::parseDoStatement() {
}

void Parser::parseReturnStatement() {
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