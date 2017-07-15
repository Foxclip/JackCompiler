#include <iostream>
#include "tokenizer.h"
#include "parser.h"
#include "debug.h"

std::string setOutputFile(std::string name) {
    std::string outputFilename = name + ".xml";
    std::cout << "Output file: " + outputFilename << std::endl;
    std::ofstream clear1(outputFilename, std::ios::trunc);
    std::ofstream clear2("debug.txt", std::ios::trunc);
    return outputFilename;
}

int main(int argc, char *argv[]) {

    std::string name(argv[1]);
    std::string outputFilename = setOutputFile(name.substr(0, name.rfind(".")));
    Tokenizer tokenizer;
    tokenizer.tokenize(argv[1]);
    //tokenizer.printTokens();
    Parser parser;
    parser.parse(outputFilename, tokenizer);

    return 0;

}