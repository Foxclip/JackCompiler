#pragma once

#include <iostream>
#include <string>

enum DebugLayer {
    SYMBOLS,
    TOKENS,
    NONE
};

extern int activeDebugPrintLayer;

void debugPrint(std::string str, DebugLayer layer);
void debugPrintLine(std::string str, DebugLayer layer);