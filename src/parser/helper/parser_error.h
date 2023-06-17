#ifndef azura_parser_error_h
#define azura_parser_error_h

#include "../lexer/tokens.h"
#include "parser_struct.h"
#include "import.h"

Parser parser;

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

static void error_parser(Token* token, const char* message) {
    // [line: 1, column: 1] Error: Unexpected character.
    std::cout << "\n[" << colorize(YELLOW) <<"line: " << colorize(CYAN) << token->line
              << "\033[0m] [" << colorize(YELLOW) <<"pos: "  << colorize(CYAN) << token->column - 1 
              << "\033[0m] Parser Error!" << "\n";

    // iterator over the tokens in the current line
    const char* line_start = token->start;
    const char* line_end = line_start;
    while (*line_end != '\n' && *line_end != '\0') line_end++;

    // Print the line
    std::cout << std::string(line_start, line_end) << "\n";

    // Print to the error
    int num_spaces = token->column - 1;
    for (int i = 0; i < num_spaces; i++) std::cout << " ";
    std::cout << colorize(RED) << "^" << colorize(RESET);

    // Print the error message
    std::cout << message;
}

static void error(const char* message) { error_parser(&parser.previous, message); }

#endif