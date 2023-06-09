#ifndef azura_error_h
#define azura_error_h

#include <iostream>
#include <sstream>

#include "helper.h"

typedef enum {
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    RESET
} Color;

const char* colorize(Color color) {
    switch (color) {
        case RED: return "\033[0;31m";
        case GREEN: return "\033[0;32m";
        case YELLOW: return "\033[0;33m";
        case BLUE: return "\033[0;34m";
        case MAGENTA: return "\033[0;35m";
        case CYAN: return "\033[0;36m";
        case WHITE: return "\033[0;37m";
        case RESET: return "\033[0m";
        default: return "\033[0m";
    }
}

static void error_lexer(Scanner* scanner, const char* message) {
    // [line: 1, column: 1] Error: Unexpected character.
    std::cout << "\n[" << colorize(YELLOW) <<"line: " << colorize(CYAN) << scanner->line
              << "\033[0m] [" << colorize(YELLOW) <<"pos: "  << colorize(CYAN) << scanner->column - 1 
              << "\033[0m] Lexical Error!" << "\n";

    // Print the error message
    std::cout << message;
}

void errorFunction(const std::string& inputString, Color color, const std::string& additionalMessage) {
    std::ostringstream oss;
    oss << inputString << colorize(color) << additionalMessage << colorize(RESET) << std::endl;
    std::string message = oss.str();

    // Call error_lexer function with the appropriate arguments
    error_lexer(&scanner, message.c_str());

    if(is_at_end()) exit(1);
}

#endif