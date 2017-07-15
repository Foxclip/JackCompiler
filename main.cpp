#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>

enum State {
    SPACE,
    SLASH,
    COMMENT,
    ALNUM_TOKEN,
    CHAR_TOKEN,
    INT_TOKEN,
    STRING
};

enum DebugLayer {
    SYMBOLS,
    TOKENS,
    NONE
};

std::string charTokens = "{}()[].,;+-*/&|<>=-";
std::string nalnumChars = "-_.";
std::vector<std::string> tokens;

std::string outputFilename;

int activeDebugPrintLayer = TOKENS;

void debugPrint(std::string str, DebugLayer layer) {
    if(layer == activeDebugPrintLayer) {
        std::cout << str;
    }
}

void debugPrintLine(std::string str, DebugLayer layer) {
    debugPrint(str + '\n', layer);
}

void writeXML(std::string line) {
    std::ofstream stream(outputFilename, std::ios_base::app);
    stream << line.c_str() << std::endl;
}

void addCharToken(char c) {
    std::string str;
    str += c;
    tokens.push_back(str);
    debugPrintLine(str, TOKENS);
}

void addStringToken(std::string token) {
    tokens.push_back(token);
    debugPrintLine(token, TOKENS);
}

void tokenize(std::string inputFilename) {
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
    debugPrintLine("", SYMBOLS);
    debugPrintLine("", TOKENS);
    while(inputStream >> std::noskipws >> c) {
        debugPrint("State: ", SYMBOLS);
        switch(currentState) {
            case SPACE:         debugPrintLine("space",       SYMBOLS);  break;
            case SLASH:         debugPrintLine("slash",       SYMBOLS);  break;
            case COMMENT:       debugPrintLine("comment",     SYMBOLS);  break;
            case ALNUM_TOKEN:   debugPrintLine("alnum_token", SYMBOLS);  break;
            case CHAR_TOKEN:    debugPrintLine("char_token",  SYMBOLS);  break;
            case INT_TOKEN:     debugPrintLine("int_token",   SYMBOLS);  break;
            case STRING:        debugPrintLine("string",      SYMBOLS);  break;
        }
        if(c == '\n') {
            debugPrintLine("Read symbol \\n", SYMBOLS);
        } else {
            debugPrintLine(std::string("Read symbol ") + c, SYMBOLS);
        }
        if(c == '\n') {
            currentState = SPACE;
        }
        if(c == '/') {
            debugPrintLine("Slash symbol found", SYMBOLS);
            if(currentState == SLASH) {
                debugPrintLine("Comment found", SYMBOLS);
                currentState = COMMENT;
            } else if(currentState == SPACE) {
                currentState = SLASH;
            }
        }
        if(c == '"') {
            if(currentState == SPACE || currentState == CHAR_TOKEN) {
                currentState = STRING;
                stringConstantBuffer.clear();
            } else if(currentState == STRING) {
                currentState = SPACE;
                addStringToken(stringConstantBuffer);
            }
        }
        if(c != '\n' && c != '"') {
            if(currentState == STRING) {
                stringConstantBuffer += c;
            }
        }
        if(std::isspace(c)) {
            debugPrintLine("Whitespace symbol found", SYMBOLS);
            if(currentState == ALNUM_TOKEN) {
                currentState = SPACE;
                addStringToken(alphanumTokenBuffer);
            } else if(currentState == CHAR_TOKEN) {
                currentState = SPACE;
            } else if(currentState == INT_TOKEN) {
                currentState = SPACE;
                addStringToken(intTokenBuffer);
            } else if(currentState == SLASH) {
                currentState = SPACE;
                addCharToken('/');
            }
        }
        if(std::isalpha(c)) {
            debugPrintLine("Letter symbol found", SYMBOLS);
            if(currentState == SPACE || currentState == CHAR_TOKEN) {
                currentState = ALNUM_TOKEN;
                alphanumTokenBuffer.clear();
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, SYMBOLS);
            } else if(currentState == ALNUM_TOKEN) {
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, SYMBOLS);
            }
        }
        if(std::isdigit(c)) {
            debugPrintLine("Digit found", SYMBOLS);
            if(currentState == SPACE || currentState == CHAR_TOKEN) {
                currentState = INT_TOKEN;
                intTokenBuffer.clear();
                intTokenBuffer += c;
                debugPrintLine("intTokenBuffer: " + intTokenBuffer, SYMBOLS);
            } else if(currentState == ALNUM_TOKEN) {
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, SYMBOLS);
            } else if(currentState == INT_TOKEN) {
                intTokenBuffer += c;
                debugPrintLine("intTokenBuffer: " + intTokenBuffer, SYMBOLS);
            }
        }
        if(charTokens.find(c) != std::string::npos) {
            debugPrintLine("Char token found", SYMBOLS);
            if(currentState == SPACE || currentState == CHAR_TOKEN) {
                currentState = CHAR_TOKEN;
                addCharToken(c);
            } else if(currentState == ALNUM_TOKEN) {
                currentState = CHAR_TOKEN;
                addStringToken(alphanumTokenBuffer);
                addCharToken(c);
            } else if(currentState == INT_TOKEN) {
                currentState = CHAR_TOKEN;
                addStringToken(intTokenBuffer);
                addCharToken(c);
            }
         }
        if(nalnumChars.find(c) != std::string::npos) {
            if(currentState == ALNUM_TOKEN) {
                alphanumTokenBuffer += c;
                debugPrintLine("alphanumTokenBuffer: " + alphanumTokenBuffer, SYMBOLS);
            }
        }

        debugPrintLine("", SYMBOLS);

    }
}

void setOutputFile(std::string name) {
    outputFilename = name + ".xml";
    std::cout << "Output file: " + outputFilename << std::endl;
    std::ofstream clearFileContents(outputFilename, std::ios::trunc);
    clearFileContents.close();
}

int main(int argc, char *argv[]) {

    std::string name(argv[1]);
    setOutputFile(name.substr(0, name.rfind(".")));
    tokenize(argv[1]);

    return 0;

}