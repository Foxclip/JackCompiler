#include <algorithm>
#include "parser.h"

void Parser::writeXML(std::string line) {
    std::ofstream stream(outputFilename, std::ios_base::app);
    stream << line.c_str() << std::endl;
}

void Parser::eat() {
    if(tokenizer.hasMoreTokens()) {
        if(tokenizer.currentToken().type == TT_ID) {
            writeXML("<identifier> " + tokenizer.currentToken().token + " </identifier>");
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
        if(str == tokenizer.currentToken().token) {
            writeXML("<" + typeToStr(tokenizer.currentToken().type) + "> " + str + " </" + typeToStr(tokenizer.currentToken().type) + ">");
            tokenizer.advance();
        } else {
            throw std::exception(std::string("'" + tokenizer.currentToken().token + "'" + ": " + str + " expected").c_str());
        }
    } else {
        throw std::exception(std::string("'" + str + "'" + " expected, but file ended").c_str());
    }
}

void Parser::eatSome(std::vector<std::string> variants) {
    std::string variantListString;
    variantListString += variants[0];
    for(int i = 0; i < variants.size(); i++) {
        variantListString += ("or" + variantListString[i]);
    }
    if(tokenizer.hasMoreTokens()) {
        if(std::find(variants.begin(), variants.end(), tokenizer.currentToken().token) != variants.end()) {
            writeXML("<" + typeToStr(tokenizer.currentToken().type) + "> " + tokenizer.currentToken().token + " </" + typeToStr(tokenizer.currentToken().type) + ">");
            tokenizer.advance();
        } else {
            throw std::exception(std::string("'" + tokenizer.currentToken().token + "'" + ": " + variantListString + "expected").c_str());
        }
    }  else {
        throw std::exception(std::string(variantListString + " expected, but file ended").c_str());
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
    writeXML("<classVarDec>");
    eatSome({"static", "field"});
    eat();
    while(true) {
        try {
            eat(",");
            eat();
        } catch(std::exception e) {
            break;
        }
    }
    eat(";");
    writeXML("</classVarDec>");
}

void Parser::parseSubroutineDec() {
    writeXML("<subroutineDec>");
    eatSome({"constructor", "function", "method"});
    throw std::exception("");
    writeXML("</subroutineDec>");
}

void Parser::parseStatements() {
    if(tokenizer.currentToken().token == "let") {
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
    } catch(std::exception e) {
        std::cout << "Parse error: " + std::string(e.what()) << std::endl;
    }
}