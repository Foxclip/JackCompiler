#include "tokenizer.h"
#include "debug.h"

enum State {
    S_SPACE,
    S_SLASH,
    S_STAR,
    S_COMMENT,
    S_MULTILINE_COMMENT,
    S_ALNUM_TOKEN,
    S_CHAR_TOKEN,
    S_INT_TOKEN,
    S_STRING
};

const std::string charTokens = "{}()[].,;+-*/&|<>=-~";
const std::string nalnumChars = "-_.";
const std::vector<std::string> keywords = {"class", "constructor", "function", "method", "field",
                                           "static", "var", "int", "char", "boolean", "void", "true",
                                           "false", "null", "this", "let", "do", "if", "else", "while", "return"};

void Tokenizer::addCharToken(char c) {
    std::string str;
    str += c;
    tokens.push_back({str, TT_SYMBOL, currentLineNumber});
}

std::string Tokenizer::typeToStr(int type) {
    std::string typeStr;
    switch(type) {
        case TT_KEYWORD:    typeStr = "keyword";            break;
        case TT_SYMBOL:     typeStr = "symbol";             break;
        case TT_INT:        typeStr = "integerConstant";    break;
        case TT_STRING:     typeStr = "stringConstant";     break;
        case TT_IDENTIFIER: typeStr = "identifier";         break;
    }
    return typeStr;
}

void Tokenizer::addStringToken(std::string token, TokenSubType subType) {
    TokenType type;
    if(subType == ST_ALNUM) {
        type = TT_IDENTIFIER;
        for(std::string str: keywords) {
            if(str == token) {
                type = TT_KEYWORD;
                break;
            }
        }
    }
    if(subType == ST_INT) {
        type = TT_INT;
    }
    if(subType == ST_STRING) {
        type = TT_STRING;
    }
    tokens.push_back({token, type, currentLineNumber});
}

void Tokenizer::tokenize(std::string inputFilename) {
    std::fstream inputStream(inputFilename);
    if(inputStream.bad()) {
        std::cout << "Error" << std::endl;
    }
    char c;
    std::string alphanumTokenBuffer;
    std::string intTokenBuffer;
    std::string stringConstantBuffer;
    State currentState = S_SPACE;
    while(inputStream >> std::noskipws >> c) {
        debugPrintLine("", DL_SYMBOLS);
        debugPrint("State: ", DL_SYMBOLS);
        switch(currentState) {
            case S_SPACE:             debugPrintLine("space",             DL_SYMBOLS);  break;
            case S_SLASH:             debugPrintLine("slash",             DL_SYMBOLS);  break;
            case S_STAR:              debugPrintLine("star",              DL_SYMBOLS);  break;
            case S_COMMENT:           debugPrintLine("comment",           DL_SYMBOLS);  break;
            case S_MULTILINE_COMMENT: debugPrintLine("multiline_comment", DL_SYMBOLS);  break;
            case S_ALNUM_TOKEN:       debugPrintLine("alnum_token",       DL_SYMBOLS);  break;
            case S_CHAR_TOKEN:        debugPrintLine("char_token",        DL_SYMBOLS);  break;
            case S_INT_TOKEN:         debugPrintLine("int_token",         DL_SYMBOLS);  break;
            case S_STRING:            debugPrintLine("string",            DL_SYMBOLS);  break;
        }
        if(c == '\n') {
            debugPrintLine("Read symbol \\n", DL_SYMBOLS);
        } else {
            debugPrintLine(std::string("Read symbol ") + c, DL_SYMBOLS);
        }
        if(c == '\n') {
            currentLineNumber++;
            if(currentState != S_MULTILINE_COMMENT && currentState != S_STAR) {
                currentState = S_SPACE;
                continue;
            } else if(currentState == S_STAR) {
                currentState = S_MULTILINE_COMMENT;
                continue;
            }
        }
        if(c == '/') {
            debugPrintLine("Slash symbol found", DL_SYMBOLS);
            if(currentState == S_SLASH) {
                debugPrintLine("Comment found", DL_SYMBOLS);
                currentState = S_COMMENT;
                continue;
            } else if(currentState == S_SPACE) {
                currentState = S_SLASH;
                continue;
            } else if(currentState == S_STAR) {
                currentState = S_SPACE;
                continue;
            }
        }
        if(c != '/') {
            if(currentState == S_STAR) {
                currentState = S_MULTILINE_COMMENT;
                continue;
            }
        }
        if(c == '*') {
            if(currentState == S_SLASH) {
                currentState = S_MULTILINE_COMMENT;
                continue;
            } else if(currentState == S_MULTILINE_COMMENT) {
                currentState = S_STAR;
                continue;
            }
        }
        if(c == '"') {
            if(currentState == S_SPACE || currentState == S_CHAR_TOKEN) {
                currentState = S_STRING;
                stringConstantBuffer.clear();
                continue;
            } else if(currentState == S_STRING) {
                currentState = S_SPACE;
                addStringToken(stringConstantBuffer, ST_STRING);
                continue;
            }
        }
        if(c != '\n' && c != '"') {
            if(currentState == S_STRING) {
                stringConstantBuffer += c;
                continue;
            }
        }
        if(std::isspace(c)) {
            debugPrintLine("Whitespace symbol found", DL_SYMBOLS);
            if(currentState == S_ALNUM_TOKEN) {
                currentState = S_SPACE;
                addStringToken(alphanumTokenBuffer, ST_ALNUM);
                continue;
            } else if(currentState == S_CHAR_TOKEN) {
                currentState = S_SPACE;
                continue;
            } else if(currentState == S_INT_TOKEN) {
                currentState = S_SPACE;
                addStringToken(intTokenBuffer, ST_INT);
                continue;
            } else if(currentState == S_SLASH) {
                currentState = S_SPACE;
                addCharToken('/');
                continue;
            }
        }
        if(std::isalpha(c)) {
            debugPrintLine("Letter symbol found", DL_SYMBOLS);
            if(currentState == S_SLASH) {
                addCharToken('/');
                currentState = S_ALNUM_TOKEN;
                alphanumTokenBuffer.clear();
                alphanumTokenBuffer += c;
                continue;
            } else if(currentState == S_SPACE || currentState == S_CHAR_TOKEN) {
                currentState = S_ALNUM_TOKEN;
                alphanumTokenBuffer.clear();
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, DL_SYMBOLS);
                continue;
            } else if(currentState == S_ALNUM_TOKEN) {
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, DL_SYMBOLS);
                continue;
            }
        }
        if(std::isdigit(c)) {
            debugPrintLine("Digit found", DL_SYMBOLS);
            if(currentState == S_SLASH) {
                addCharToken('/');
                currentState = S_INT_TOKEN;
                alphanumTokenBuffer.clear();
                alphanumTokenBuffer += c;
                continue;
            } else if(currentState == S_SPACE || currentState == S_CHAR_TOKEN) {
                currentState = S_INT_TOKEN;
                intTokenBuffer.clear();
                intTokenBuffer += c;
                debugPrintLine("intTokenBuffer: " + intTokenBuffer, DL_SYMBOLS);
                continue;
            } else if(currentState == S_ALNUM_TOKEN) {
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, DL_SYMBOLS);
                continue;
            } else if(currentState == S_INT_TOKEN) {
                intTokenBuffer += c;
                debugPrintLine("intTokenBuffer: " + intTokenBuffer, DL_SYMBOLS);
                continue;
            }
        }
        if(charTokens.find(c) != std::string::npos) {
            debugPrintLine("Char token found", DL_SYMBOLS);
            if(currentState == S_SPACE || currentState == S_CHAR_TOKEN) {
                currentState = S_CHAR_TOKEN;
                addCharToken(c);
                continue;
            } else if(currentState == S_ALNUM_TOKEN) {
                currentState = S_CHAR_TOKEN;
                addStringToken(alphanumTokenBuffer, ST_ALNUM);
                addCharToken(c);
                continue;
            } else if(currentState == S_INT_TOKEN) {
                currentState = S_CHAR_TOKEN;
                addStringToken(intTokenBuffer, ST_INT);
                addCharToken(c);
                continue;
            }
         }
        if(nalnumChars.find(c) != std::string::npos) {
            if(currentState == S_ALNUM_TOKEN) {
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, DL_SYMBOLS);
                continue;
            }
        }
    }
    debugPrintLine("", DL_SYMBOLS);
}

bool Tokenizer::hasMoreTokens() {
    return currentTokenIndex <= tokens.size() - 1;
}

void Tokenizer::advance() {
    currentTokenIndex++;
}

Token Tokenizer::currentToken() {
    return tokens[currentTokenIndex];
}

Token Tokenizer::nextToken() {
    return tokens[currentTokenIndex + 1];
}

void Tokenizer::printTokens() {
    for(Token token: tokens) {
        std::cout << "'" << token.token << "' : " << typeToStr(token.type) << std::endl;
    }
}