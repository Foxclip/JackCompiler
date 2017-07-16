#pragma once

#include <iostream>
#include <string>
#include <fstream>

enum DebugLayer {
    DL_SYMBOLS,
    DL_PARSER,
    DL_NONE
};

extern int activeDebugPrintLayer;

void debugPrint(std::string str, DebugLayer layer);
void debugPrintLine(std::string str, DebugLayer layer);