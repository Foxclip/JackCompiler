#include "tokenizer.h"
#include "debug.h"

enum State {
    SPACE,
    SLASH,
    STAR,
    COMMENT,
    MULTILINE_COMMENT,
    ALNUM_TOKEN,
    CHAR_TOKEN,
    INT_TOKEN,
    STRING
};

const std::string charTokens = "{}()[].,;+-*/&|<>=-~";
const std::string nalnumChars = "-_.";

void Tokenizer::addCharToken(char c) {
    std::string str;
    str += c;
    tokens.push_back(str);
    debugPrint(str + " ", TOKENS);
}

void Tokenizer::addStringToken(std::string token) {
    tokens.push_back(token);
    debugPrint(token + " ", TOKENS);
}

void Tokenizer::tokenize(std::string inputFilename) {
    std::cout << "Tokenizing " + inputFilename << std::endl;
    std::fstream inputStream(inputFilename);
    if(inputStream.bad()) {
        std::cout << "Error" << std::endl;
    }
    char c;
    std::string alphanumTokenBuffer;
    std::string intTokenBuffer;
    std::string stringConstantBuffer;
    State currentState = SPACE;
    while(inputStream >> std::noskipws >> c) {
        debugPrintLine("", SYMBOLS);
        debugPrint("State: ", SYMBOLS);
        switch(currentState) {
            case SPACE:             debugPrintLine("space",             SYMBOLS);  break;
            case SLASH:             debugPrintLine("slash",             SYMBOLS);  break;
            case STAR:              debugPrintLine("star",              SYMBOLS);  break;
            case COMMENT:           debugPrintLine("comment",           SYMBOLS);  break;
            case MULTILINE_COMMENT: debugPrintLine("multiline_comment", SYMBOLS);  break;
            case ALNUM_TOKEN:       debugPrintLine("alnum_token",       SYMBOLS);  break;
            case CHAR_TOKEN:        debugPrintLine("char_token",        SYMBOLS);  break;
            case INT_TOKEN:         debugPrintLine("int_token",         SYMBOLS);  break;
            case STRING:            debugPrintLine("string",            SYMBOLS);  break;
        }
        if(c == '\n') {
            debugPrintLine("Read symbol \\n", SYMBOLS);
        } else {
            debugPrintLine(std::string("Read symbol ") + c, SYMBOLS);
        }
        if(c == '\n') {
            if(currentState != MULTILINE_COMMENT && currentState != STAR) {
                currentState = SPACE;
                continue;
            } else if(currentState == STAR) {
                currentState = MULTILINE_COMMENT;
                continue;
            }
        }
        if(c == '/') {
            debugPrintLine("Slash symbol found", SYMBOLS);
            if(currentState == SLASH) {
                debugPrintLine("Comment found", SYMBOLS);
                currentState = COMMENT;
                continue;
            } else if(currentState == SPACE) {
                currentState = SLASH;
                continue;
            } else if(currentState == STAR) {
                currentState = SPACE;
                continue;
            }
        }
        if(c != '/') {
            if(currentState == STAR) {
                currentState = MULTILINE_COMMENT;
                continue;
            }
        }
        if(c == '*') {
            if(currentState == SLASH) {
                currentState = MULTILINE_COMMENT;
                continue;
            } else if(currentState == MULTILINE_COMMENT) {
                currentState = STAR;
                continue;
            }
        }
        if(c == '"') {
            if(currentState == SPACE || currentState == CHAR_TOKEN) {
                currentState = STRING;
                stringConstantBuffer.clear();
                continue;
            } else if(currentState == STRING) {
                currentState = SPACE;
                addStringToken(stringConstantBuffer);
                continue;
            }
        }
        if(c != '\n' && c != '"') {
            if(currentState == STRING) {
                stringConstantBuffer += c;
                continue;
            }
        }
        if(std::isspace(c)) {
            debugPrintLine("Whitespace symbol found", SYMBOLS);
            if(currentState == ALNUM_TOKEN) {
                currentState = SPACE;
                addStringToken(alphanumTokenBuffer);
                continue;
            } else if(currentState == CHAR_TOKEN) {
                currentState = SPACE;
                continue;
            } else if(currentState == INT_TOKEN) {
                currentState = SPACE;
                addStringToken(intTokenBuffer);
                continue;
            } else if(currentState == SLASH) {
                currentState = SPACE;
                addCharToken('/');
                continue;
            }
        }
        if(std::isalpha(c)) {
            debugPrintLine("Letter symbol found", SYMBOLS);
            if(currentState == SLASH) {
                addCharToken('/');
                currentState = ALNUM_TOKEN;
                alphanumTokenBuffer.clear();
                alphanumTokenBuffer += c;
                continue;
            } else if(currentState == SPACE || currentState == CHAR_TOKEN) {
                currentState = ALNUM_TOKEN;
                alphanumTokenBuffer.clear();
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, SYMBOLS);
                continue;
            } else if(currentState == ALNUM_TOKEN) {
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, SYMBOLS);
                continue;
            }
        }
        if(std::isdigit(c)) {
            debugPrintLine("Digit found", SYMBOLS);
            if(currentState == SLASH) {
                addCharToken('/');
                currentState = INT_TOKEN;
                alphanumTokenBuffer.clear();
                alphanumTokenBuffer += c;
                continue;
            } else if(currentState == SPACE || currentState == CHAR_TOKEN) {
                currentState = INT_TOKEN;
                intTokenBuffer.clear();
                intTokenBuffer += c;
                debugPrintLine("intTokenBuffer: " + intTokenBuffer, SYMBOLS);
                continue;
            } else if(currentState == ALNUM_TOKEN) {
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, SYMBOLS);
                continue;
            } else if(currentState == INT_TOKEN) {
                intTokenBuffer += c;
                debugPrintLine("intTokenBuffer: " + intTokenBuffer, SYMBOLS);
                continue;
            }
        }
        if(charTokens.find(c) != std::string::npos) {
            debugPrintLine("Char token found", SYMBOLS);
            if(currentState == SPACE || currentState == CHAR_TOKEN) {
                currentState = CHAR_TOKEN;
                addCharToken(c);
                continue;
            } else if(currentState == ALNUM_TOKEN) {
                currentState = CHAR_TOKEN;
                addStringToken(alphanumTokenBuffer);
                addCharToken(c);
                continue;
            } else if(currentState == INT_TOKEN) {
                currentState = CHAR_TOKEN;
                addStringToken(intTokenBuffer);
                addCharToken(c);
                continue;
            }
         }
        if(nalnumChars.find(c) != std::string::npos) {
            if(currentState == ALNUM_TOKEN) {
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, SYMBOLS);
                continue;
            }
        }
    }
}
