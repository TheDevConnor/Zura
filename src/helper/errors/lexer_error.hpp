#pragma once

#include "../../../inc/colorize.hpp"
#include "../../lexer/lexer.hpp"
#include "../../common.hpp"

class LexerError {
private:
public:
    inline static void error_lexer(Scanner* scanner, std::string message) {
        std::cout << "\n[" << termcolor::yellow << "line: " << termcolor::cyan << scanner->line
                  << termcolor::reset << "] [" << termcolor::yellow << "column: " 
                  << termcolor::cyan << scanner->column - 1 << termcolor::reset << "] "
                  << termcolor::red << "Lexer Error: \n" 
                  << termcolor::reset << message << std::endl;
        ZuraExit(LEXER_ERROR);
    }
};