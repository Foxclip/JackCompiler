#include <algorithm>
#include "compiler.h"

void Compiler::writeXML(std::string line) {
    int braceCount = 0;
    bool indentDone = false;
    char previousChar = 0;
    for(char c: line) {
        if(c == '<' || c == '>') {
            braceCount++;
        }
    }
    if(braceCount == 2) {
        for(char c: line) {
            if(c == '/' && previousChar == '<') {
                xmlIndentLevel -= 1;
                indentDone = true;
                break;
            }
            previousChar = c;
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
            if(c == '/' && previousChar == '<') {
                xmlIndentLevel -= 2;
            }
            previousChar = c;
        }
    }
}

std::string Compiler::tokenName() {
    return tokenizer.currentToken().token;
}

int Compiler::tokenType() {
    return tokenizer.currentToken().type;
}

std::string xmlReplace(std::string str) {
    if(str.size() == 1) {
        switch(str[0]) {
            case '<':  return "&lt;";   break;
            case '>':  return "&gt;";   break;
            case '\"': return "&quot;"; break;
            case '&':  return "&amp;";  break;
            default:   return str;
        }
    } else {
        return str;
    }
}

void Compiler::eat(bool valid, std::string whatExpected) {
    if(tokenizer.hasMoreTokens()) {
        if(valid) {
            writeXML("<" + tokenizer.typeToStr(tokenType()) + "> " + xmlReplace(tokenName()) + " </" + tokenizer.typeToStr(tokenType()) + ">");
            tokenizer.advance();
        } else {
            throw SyntaxError(std::string("'" + tokenName() + "'" + ": " + whatExpected + " expected").c_str());
        }
    } else {
        throw SyntaxError(std::string(whatExpected + " expected, but file ended").c_str());
    }
}

void Compiler::eatIdentifier() {
    eat(tokenType() == TT_IDENTIFIER, "identifier");
}

void Compiler::eatType() {
    eat(tokenName() == "int" || tokenName() == "char" || tokenName() == "boolean" || tokenType() == TT_IDENTIFIER, "type");
}

void Compiler::eatStr(std::string str) {
    eat(str == tokenName(), str);
}

void Compiler::compileClass() {
    writeXML("<class>");
    eatStr("class");
    eatIdentifier();
    eatStr("{");
    while(true) {
        try {
            compileClassVarDec();
        } catch(SyntaxError e) {
            break;
        }
    }
    while(true) {
        try {
            compileSubroutineDec();
        } catch(SyntaxError e) {
            break;
        }
    }
    eatStr("}");
    writeXML("</class>");
}

void Compiler::compileClassVarDec() {
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

void Compiler::compileSubroutineDec() {
    if(tokenName() == "constructor" || tokenName() == "function" || tokenName() == "method") {
        writeXML("<subroutineDec>");
    }
    eat(tokenName() == "constructor" || tokenName() == "function" || tokenName() == "method", "'constructor', 'function' or 'method'");
    eat(tokenName() == "void" || tokenName() == "int" || tokenName() == "char" || tokenName() == "boolean" || tokenType() == TT_IDENTIFIER, "'void' or type");
    eatIdentifier();
    eatStr("(");
    compileParameterList();
    eatStr(")");
    compileSubroutineBody();
    writeXML("</subroutineDec>");
}

void Compiler::compileParameterList() {
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

void Compiler::compileSubroutineBody() {
    writeXML("<subroutineBody>");
    eatStr("{");
    while(true) {
        try {
            compileVarDec();
        } catch(SyntaxError e) {
            break;
        }
    }
    compileStatements();
    eatStr("}");
    writeXML("</subroutineBody>");
}

void Compiler::compileVarDec() {
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

void Compiler::compileStatements() {
    if(tokenName() == "let" || tokenName() == "if" || tokenName() == "while" || tokenName() == "do" || tokenName() == "return") {
        writeXML("<statements>");
    }
    while(true) {
        if(tokenName() == "let") {
            compileLetStatement();
        } else if(tokenName() == "if") {
            compileIfStatement();
        } else if(tokenName() == "while") {
            compileWhileStatement();
        } else if(tokenName() == "do") {
            compileDoStatement();
        } else if(tokenName() == "return") {
            compileReturnStatement();
        } else {
            break;
        }
    }
    writeXML("</statements>");
}

void Compiler::compileLetStatement() {
    writeXML("<letStatement>");
    eatStr("let");
    eatIdentifier();
    try {
        eatStr("[");
        compileExpression();
        eatStr("]");
    } catch(SyntaxError e) {}
    eatStr("=");
    compileExpression();
    eatStr(";");
    writeXML("</letStatement>");
}

void Compiler::compileIfStatement() {
    writeXML("<ifStatement>");
    eatStr("if");
    eatStr("(");
    compileExpression();
    eatStr(")");
    eatStr("{");
    compileStatements();
    eatStr("}");
    try {
        eatStr("else");
        eatStr("{");
        compileStatements();
        eatStr("}");
    } catch(SyntaxError e) {}
    writeXML("</ifStatement>");
}

void Compiler::compileWhileStatement() {
    writeXML("<whileStatement>");
    eatStr("while");
    eatStr("(");
    compileExpression();
    eatStr(")");
    eatStr("{");
    compileStatements();
    eatStr("}");
    writeXML("</whileStatement>");
}

void Compiler::compileDoStatement() {
    writeXML("<doStatement>");
    eatStr("do");
    compileSubroutineCall();
    eatStr(";");
    writeXML("</doStatement>");
}

void Compiler::compileReturnStatement() {
    writeXML("<returnStatement>");
    eatStr("return");
    try {
        compileExpression();
    } catch(SyntaxError e) {}
    eatStr(";");
    writeXML("</returnStatement>");
}

void Compiler::compileExpression() {
    if(tokenName() == ")" || tokenName() == ";") {
        return;
    }
    writeXML("<expression>");
    compileTerm();
    while(true) {
        try {
            compileOp();
            compileTerm();
        } catch(SyntaxError e) {
            break;
        }
    }
    writeXML("</expression>");
}

void Compiler::compileExpressionList() {
    writeXML("<expressionList>");
    try {
        compileExpression();
        while(true) {
            try {
                eatStr(",");
                compileExpression();
            } catch(SyntaxError e) {
                break;
            }
        }
    } catch(SyntaxError e) {}
    writeXML("</expressionList>");
}

void Compiler::compileTerm() {
    writeXML("<term>");
    if(tokenType() == TT_INT || tokenType() == TT_STRING || tokenType() == TT_KEYWORD) {
        eatStr(tokenName());
    } else if(tokenType() == TT_IDENTIFIER) {
        if(tokenizer.hasMoreTokens()) {
            if(tokenizer.nextToken().token == "[") {
                eatIdentifier();
                eatStr("[");
                compileExpression();
                eatStr("]");
            } else if(tokenizer.nextToken().token == "(" || tokenizer.nextToken().token == ".") {
                compileSubroutineCall();
            } else {
                eatIdentifier();
            }
        } else {
            eatIdentifier();
        }
    } else if(tokenName() == "(") {
        eatStr("(");
        compileExpression();
        eatStr(")");
    } else if(tokenName() == "-" || tokenName() == "~") {
        eatStr(tokenName());
        compileTerm();
    }
    writeXML("</term>");
}

void Compiler::compileOp() {
    eat(std::string("+-*/&|<>=").find(tokenName()[0]) != std::string::npos, "binary operator");
}

void Compiler::compileSubroutineCall() {
    eatIdentifier();
    if(tokenName() == "(") {
        eatStr("(");
        compileExpressionList();
        eatStr(")");
    }
    if(tokenName() == ".") {
        eatStr(".");
        eatIdentifier();
        eatStr("(");
        compileExpressionList();
        eatStr(")");
    }
}

std::string setOutputFile(std::string name) {
    std::string outputFilename = name + ".xml";
    std::ofstream clear1(outputFilename, std::ios::trunc);
    return outputFilename;
}

void Compiler::compile(std::string inputFilename) {
    outputFilename = setOutputFile(inputFilename.substr(0, inputFilename.rfind(".")));
    tokenizer = Tokenizer();
    tokenizer.tokenize(inputFilename);
    std::cout << "Compiling " + inputFilename << std::endl;
    try {
        compileClass();
        debugPrintLine("Succesfully compiled", DL_COMPILER);
        debugPrintLine("", DL_COMPILER);
    } catch(CompileError e) {
        std::cout << "Compile error: " + std::string(e.what()) << std::endl;
        std::cout << std::endl;
    }
}