#include <iostream>
#include "dirent.h"
#include "tokenizer.h"
#include "Compiler.h"
#include "debug.h"

bool endsWith(std::string const &fullString, std::string const &ending) {
    if(fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

int main(int argc, char *argv[]) {

    std::string inputName(argv[1]);
    if(inputName.back() == '/' || inputName.back() == '\\') {
        DIR *dir;
        struct dirent *ent;
        std::string dirName = inputName.substr(0, inputName.size() - 1);
        if((dir = opendir(dirName.c_str())) != NULL) {
            while((ent = readdir(dir)) != NULL) {
                if(ent->d_type == DT_REG && endsWith(ent->d_name, ".jack")) {
                    Compiler compiler;
                    compiler.compile(inputName + ent->d_name);
                }
            }
            closedir(dir);
        } else {
            perror("Error");
        }
    } else {
        Compiler compiler;
        compiler.compile(inputName);
    }

    return 0;

}