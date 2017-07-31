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
    std::ofstream stream(outputXMLFilename, std::ios_base::app);
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

void Compiler::writeVM(std::string line) {
    std::ofstream stream(outputVMFilename, std::ios_base::app);
    stream << line << std::endl;
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

std::string Compiler::eat(bool valid, std::string whatExpected) {
    std::string result = tokenName();
    if(tokenizer.hasMoreTokens()) {
        if(valid) {
            writeXML("<" + tokenizer.typeToStr(tokenType()) + "> " + xmlReplace(tokenName()) + " </" + tokenizer.typeToStr(tokenType()) + ">");
            tokenizer.advance();
        } else {
            throw SyntaxError(std::string("Line " + std::to_string(tokenizer.currentToken().lineNumber) + ": '" + tokenName() + "'" + ": " + whatExpected + " expected").c_str());
        }
    } else {
        throw SyntaxError(std::string(whatExpected + " expected, but file ended").c_str());
    }
    return result;
}

std::string Compiler::eatIdentifier() {
    return eat(tokenType() == TT_IDENTIFIER, "identifier");
}

std::string Compiler::eatType() {
    return eat(tokenName() == "int" || tokenName() == "char" || tokenName() == "boolean" || tokenType() == TT_IDENTIFIER, "type");
}

std::string Compiler::eatStr(std::string str) {
    return eat(str == tokenName(), str);
}

SymbolTableEntry Compiler::findInSymbolTables(std::string name) {
    for(SymbolTableEntry entry: subroutineSymbolTable) {
        if(entry.name == name) {
            return entry;
        }
    }
    for(SymbolTableEntry entry: classSymbolTable) {
        if(entry.name == name) {
            return entry;
        }
    }
    return {"null", "null", "null", -1};
}

void Compiler::compileClass() {
    writeXML("<class>");
    eatStr("class");
    className = eatIdentifier();
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
    std::string varKind = eat(tokenName() == "static" || tokenName() == "field", "'static' or 'field'");
    std::string varType = eatType();
    std::string varName = eatIdentifier();
    std::vector<std::string> varNameList;
    varNameList.push_back(varName);
    while(true) {
        try {
            eatStr(",");
            std::string additionalVarName = eatIdentifier();
            varNameList.push_back(additionalVarName);
        } catch(SyntaxError e) {
            break;
        }
    }
    for(int i = 0; i < (int)varNameList.size(); i++) {
        int varIndex;
        if(varKind == "field" || varKind == "this") {
            varKind = "this";
            varIndex = classFieldCount;
            classFieldCount++;
        } else if(varKind == "static") {
            varIndex = classStaticCount;
            classStaticCount++;
        } else {
            debugPrintLine("oops... " + varKind + " | " + varType + " | " + varName, DL_COMPILER);
        }
        classSymbolTable.push_back({varNameList[i], varType, varKind, varIndex});
    }
    eatStr(";");
    writeXML("</classVarDec>");
}

void Compiler::compileSubroutineDec() {
    if(tokenName() == "constructor" || tokenName() == "function" || tokenName() == "method") {
        writeXML("<subroutineDec>");
    }
    subroutineKind = eat(tokenName() == "constructor" || tokenName() == "function" || tokenName() == "method", "'constructor', 'function' or 'method'");
    std::string returnType = eat(tokenName() == "void" || tokenName() == "int" || tokenName() == "char" || tokenName() == "boolean" || tokenType() == TT_IDENTIFIER, "'void' or type");
    subroutineName = eatIdentifier();
    subroutineSymbolTable.clear();
    subroutineArgCount = 0;
    subroutineLocalCount = 0;
    if(subroutineKind == "method") {
        subroutineSymbolTable.push_back({"this", className, "argument", 0});
        subroutineArgCount++;
    }
    eatStr("(");
    compileParameterList();
    eatStr(")");
    compileSubroutineBody();
    writeXML("</subroutineDec>");
    writeVM("");
    writeVM("");
    writeVM("");
}

void Compiler::addArgument() {
    std::string argType = eatType();
    std::string argName = eatIdentifier();
    subroutineSymbolTable.push_back({argName, argType, "argument", subroutineArgCount});
    subroutineArgCount++;
}

void Compiler::compileParameterList() {
    writeXML("<parameterList>");
    try {
        addArgument();
        while(true) {
            try {
                eatStr(",");
                addArgument();
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
    writeVM("function " + className + "." + subroutineName + " " + std::to_string(subroutineLocalCount));
    writeVM("");
    if(subroutineKind == "constructor") {
        writeVM("push constant " + std::to_string(classFieldCount));
        writeVM("call Memory.alloc 1");
        writeVM("pop pointer 0");
        writeVM("");
    } else if(subroutineKind == "method") {
        writeVM("push argument 0");
        writeVM("pop pointer 0");
        writeVM("");
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
    std::string varType = eatType();
    std::string varName = eatIdentifier();
    subroutineSymbolTable.push_back({varName, varType, "local", subroutineLocalCount});
    subroutineLocalCount++;
    while(true) {
        try {
            eatStr(",");
            varName = eatIdentifier();
            subroutineSymbolTable.push_back({varName, varType, "local", subroutineLocalCount});
            subroutineLocalCount++;
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
    std::string varName = eatIdentifier();
    SymbolTableEntry entry = findInSymbolTables(varName);
    if(entry.index == -1) {
        throw SemanticError("Line " + std::to_string(tokenizer.currentToken().lineNumber) + ": " + "variable '" + varName + "' is undefined");
    }
    bool arraySet = false;
    try {
        eatStr("[");
        arraySet = true;
        writeVM("push " + entry.kind + " " + std::to_string(entry.index));
        compileExpression();
        writeVM("add");
        eatStr("]");
    } catch(SyntaxError e) {}
    eatStr("=");
    compileExpression();
    eatStr(";");
    if(arraySet) {
        writeVM("pop temp 0");
        writeVM("pop pointer 1");
        writeVM("push temp 0");
        writeVM("pop that 0");
    } else {
        writeVM("pop " + entry.kind + " " + std::to_string(entry.index));
    }
    writeXML("</letStatement>");
    writeVM("");
}

void Compiler::compileIfStatement() {
    std::string labelL1 = className + "_ifL1." + std::to_string(runningIndex);
    std::string labelL2 = className + "_ifL2." + std::to_string(runningIndex);
    runningIndex++;
    writeXML("<ifStatement>");
    eatStr("if");
    eatStr("(");
    compileExpression();
    eatStr(")");
    writeVM("not");
    writeVM("if-goto " + labelL1);
    writeVM("");
    eatStr("{");
    compileStatements();
    eatStr("}");
    writeVM("goto " + labelL2);
    writeVM("label " + labelL1);
    writeVM("");
    try {
        eatStr("else");
        eatStr("{");
        compileStatements();
        eatStr("}");
    } catch(SyntaxError e) {}
    writeVM("label " + labelL2);
    writeXML("</ifStatement>");
    writeVM("");
}

void Compiler::compileWhileStatement() {
    std::string labelL1 = className + "_whileL1." + std::to_string(runningIndex);
    std::string labelL2 = className + "_whileL2." + std::to_string(runningIndex);
    runningIndex++;
    writeVM("label " + labelL1);
    writeVM("");
    writeXML("<whileStatement>");
    eatStr("while");
    eatStr("(");
    compileExpression();
    eatStr(")");
    writeVM("not");
    writeVM("if-goto " + labelL2);
    writeVM("");
    eatStr("{");
    compileStatements();
    eatStr("}");
    writeVM("goto " + labelL1);
    writeVM("label " + labelL2);
    writeXML("</whileStatement>");
    writeVM("");
}

void Compiler::compileDoStatement() {
    writeXML("<doStatement>");
    eatStr("do");
    compileSubroutineCall();
    eatStr(";");
    writeVM("pop temp 0");
    writeXML("</doStatement>");
    writeVM("");
}

void Compiler::compileReturnStatement() {
    writeXML("<returnStatement>");
    eatStr("return");
    bool isEmpty;
    try {
        isEmpty = compileExpression();
    } catch(SyntaxError e) {}
    eatStr(";");
    writeXML("</returnStatement>");
    if(isEmpty) {
        writeVM("push constant 0");
    }
    writeVM("return");
    writeVM("");
}

bool Compiler::compileExpression() {
    if((tokenName() == ")" || tokenName() == ";") && tokenType() == TT_SYMBOL) {
        return true;
    }
    writeXML("<expression>");
    compileTerm();
    while(true) {
        try {
            std::string func = compileOp();
            compileTerm();
            writeVM(func);
        } catch(SyntaxError e) {
            break;
        }
    }
    writeXML("</expression>");
    return false;
}

int Compiler::compileExpressionList() {
    int expressionCount = 0;
    writeXML("<expressionList>");
    try {
        bool empty = compileExpression();
        if(!empty) {
            expressionCount++;
        }
        while(true) {
            try {
                eatStr(",");
                empty = compileExpression();
                if(!empty) {
                    expressionCount++;
                }
            } catch(SyntaxError e) {
                break;
            }
        }
    } catch(SyntaxError e) {}
    writeXML("</expressionList>");
    return expressionCount;
}

void Compiler::compileTerm() {
    writeXML("<term>");
    if(tokenType() == TT_INT) {
        writeVM("push constant " + tokenName());
        eatStr(tokenName());
    } else if(tokenType() == TT_STRING) {
        writeVM("push constant " + std::to_string(tokenName().size()));
        writeVM("call String.new 1");
        for(char c: tokenName()) {
            writeVM("push constant " + std::to_string(c));
            writeVM("call String.appendChar 2");
        }
        writeVM("");
        eatStr(tokenName());
    } else if(tokenType() == TT_KEYWORD) {
        if(tokenName() == "true") {
            writeVM("push constant 1");
            writeVM("neg");
        } else if(tokenName() == "false") {
            writeVM("push constant 0");
        } else if(tokenName() == "this") {
            writeVM("push pointer 0");
        } else if(tokenName() == "null") {
            writeVM("push constant 0");
        } else {
            throw SemanticError("'" + tokenName() + "' is not allowed here");
        }
        eatStr(tokenName());
    } else if(tokenType() == TT_IDENTIFIER) {
        if(tokenizer.hasMoreTokens()) {
            if(tokenizer.nextToken().token == "[") {
                std::string varName = eatIdentifier();
                eatStr("[");
                SymbolTableEntry entry = findInSymbolTables(varName);
                if(entry.index != -1) {
                    writeVM("push " + entry.kind + " " + std::to_string(entry.index));
                } else {
                    throw SemanticError("Line " + std::to_string(tokenizer.currentToken().lineNumber) + ": " + "variable '" + varName + "' is undefined");
                }
                compileExpression();
                writeVM("add");
                writeVM("pop pointer 1");
                writeVM("push that 0");
                eatStr("]");
            } else if(tokenizer.nextToken().token == "(" || tokenizer.nextToken().token == ".") {
                compileSubroutineCall();
            } else {
                std::string varName = eatIdentifier();
                SymbolTableEntry entry = findInSymbolTables(varName);
                if(entry.index != -1) {
                    if(entry.kind == "this") {
                        writeVM("push pointer 0");                               //current object
                        writeVM("pop temp 1");                                   //saved copy
                        writeVM("push pointer 0");                               //current object
                        writeVM("push constant " + std::to_string(entry.index)); //field index
                        writeVM("add");                                          //add
                        writeVM("pop pointer 0");                                //set pointer 0 to desired field
                        writeVM("push this 0");                                  //push field to the stack
                        writeVM("push temp 1");                                  //saved copy
                        writeVM("pop pointer 0");                                //restore pointer 0
                        writeVM("");
                    } else {
                        writeVM("push " + entry.kind + " " + std::to_string(entry.index));
                    }
                } else {
                    throw SemanticError("Line " + std::to_string(tokenizer.currentToken().lineNumber) + ": " + "variable '" + varName + "' is undefined");
                }
            }
        } else {
            eatIdentifier();
        }
    } else if(tokenName() == "(") {
        eatStr("(");
        compileExpression();
        eatStr(")");
    } else if(tokenName() == "-") {
        eatStr(tokenName());
        compileTerm();
        writeVM("neg");
    } else if(tokenName() == "~") {
        eatStr(tokenName());
        compileTerm();
        writeVM("not");
    }
    writeXML("</term>");
}

std::string Compiler::compileOp() {
    std::string func;
    switch(tokenName()[0]) {
        case '+': func = "add";                  break;
        case '-': func = "sub";                  break;
        case '*': func = "call Math.multiply 2"; break;
        case '/': func = "call Math.divide 2";   break;
        case '&': func = "and";                  break;
        case '|': func = "or";                   break;
        case '<': func = "lt";                   break;
        case '>': func = "gt";                   break;
        case '=': func = "eq";                   break;
    }
    eat(std::string("+-*/&|<>=").find(tokenName()[0]) != std::string::npos, "binary operator");
    return func;
}

void Compiler::compileSubroutineCall() {
    std::string calledClassName, calledSubroutineName;
    std::string firstIdentifier = eatIdentifier();
    if(tokenName() == "(") {
        calledSubroutineName = firstIdentifier;
        int parameterCount = 0;
        if(subroutineKind == "method") {
            parameterCount = 1;
            writeVM("push pointer 0");
        }
        eatStr("(");
        parameterCount += compileExpressionList();
        eatStr(")");
        writeVM("call " + className + "." + calledSubroutineName + " " + std::to_string(parameterCount));
    }
    if(tokenName() == ".") {
        calledClassName = firstIdentifier;
        bool found = false;
        SymbolTableEntry entry = findInSymbolTables(calledClassName);
        if(entry.index != -1) {
            writeVM("push " + entry.kind + " " + std::to_string(entry.index));
            found = true;
        }
        eatStr(".");
        calledSubroutineName = eatIdentifier();
        eatStr("(");
        int parameterCount = compileExpressionList();
        if(found) {
            parameterCount++;
        }
        eatStr(")");
        std::string typeStr = calledClassName;
        if(found) {
            typeStr = entry.type;
        }
        writeVM("call " + typeStr + "." + calledSubroutineName + " " + std::to_string(parameterCount));
    }
}

void Compiler::compile(std::string inputFilename) {
    std::string name = inputFilename.substr(0, inputFilename.rfind("."));
    std::string individualFilename = inputFilename.substr(inputFilename.rfind("/") + 1, inputFilename.size() - 1);
    outputXMLFilename = name + ".xml";
    outputVMFilename = name + ".vm";
    std::ofstream clear1(outputXMLFilename, std::ios::trunc);
    std::ofstream clear2(outputVMFilename, std::ios::trunc);
    clear1.close();
    clear2.close();
    tokenizer = Tokenizer();
    tokenizer.tokenize(inputFilename);
    //tokenizer.printTokens();
    std::cout << "Compiling " + individualFilename << std::endl;
    try {
        compileClass();
    } catch(CompileError e) {
        std::cout << "Compile error: " + std::string(e.what()) << std::endl;
        std::cout << std::endl;
    }
}
