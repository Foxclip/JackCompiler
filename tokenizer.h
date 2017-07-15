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
    TT_ID
};

struct Token {
    std::string token;
    TokenType type;
};

std::string typeToStr(TokenType type);

class Tokenizer {
public:

    void tokenize(std::string inputFilename);
    bool hasMoreTokens();
    void advance();
    Token getToken();
    void printTokens();

private:

    std::vector<Token> tokens;
    int currentToken = 0;

    void addCharToken(char c);
    void addStringToken(std::string token, TokenSubType subType);

};