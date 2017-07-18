#include "debug.h"

int activeDebugPrintLayer = DL_COMPILER;

void debugPrint(std::string str, DebugLayer layer) {
    if(layer == activeDebugPrintLayer) {
        std::cout << str;
        //std::ofstream stream("debug.txt", std::ios_base::app);
        //stream << str.c_str();
    }
}

void debugPrintLine(std::string str, DebugLayer layer) {
    debugPrint(str + '\n', layer);
}