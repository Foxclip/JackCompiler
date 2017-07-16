#pragma once

#include <fstream>
#include <string>
#include <cctype>
#include <vector>

enum TokenSubType {
    ST_ALNUM,
    ST_STRING,
    ST_INT
};

enum TokenType {
    TT_KEYWORD,
    TT_SYMBOL,
    TT_INT,
    TT_STRING,
    TT_IDENTIFIER
};

struct Token {
    std::string token;
    int type;
};

class Tokenizer {

public:
    void tokenize(std::string inputFilename);
    bool hasMoreTokens();
    void advance();
    Token currentToken();
    Token nextToken();
    void printTokens();
    std::string typeToStr(int type);

private:
    std::vector<Token> tokens;
    int currentTokenIndex = 0;

    void addCharToken(char c);
    void addStringToken(std::string token, TokenSubType subType);

};