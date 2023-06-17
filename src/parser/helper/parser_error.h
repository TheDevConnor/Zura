#ifndef azura_parser_error_h
#define azura_parser_error_h

#include "../lexer/tokens.h"
#include "parser.h"
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
    parser.had_error = true;
    if(parser.panic_mode) return;
    parser.panic_mode = true;

    std::printf("\n[%sline: %s%d%s, %spos: %s%d%s] Error: ", 
        colorize(YELLOW), colorize(CYAN), token->line, colorize(YELLOW), 
        colorize(YELLOW), colorize(CYAN), token->column - 1, colorize(YELLOW));

    if (token->kind == EOF_TOKEN) std::printf("at end \n");
    else if (token->kind == ERROR_TOKEN) {}
    else std::printf("at '%.*s'\n", token->length, token->start);

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

static void error_at_current(const char* message) { error_parser(&parser.current, message); }
static void error(const char* message) { error_parser(&parser.previous, message); }

#endif