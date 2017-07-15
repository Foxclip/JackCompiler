#pragma once

#include <iostream>
#include <string>

enum DebugLayer {
    DL_SYMBOLS,
    DL_NONE
};

extern int activeDebugPrintLayer;

void debugPrint(std::string str, DebugLayer layer);
void debugPrintLine(std::string str, DebugLayer layer);