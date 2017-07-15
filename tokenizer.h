#pragma once

#include <fstream>
#include <string>
#include <cctype>
#include <vector>

class Tokenizer {
public:

    void tokenize(std::string inputFilename);

private:

    std::vector<std::string> tokens;

    void addCharToken(char c);
    void addStringToken(std::string token);

};