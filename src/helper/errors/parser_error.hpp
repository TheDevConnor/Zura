#pragma once

#include <iostream>
#include <cstring>

#include "../../../inc/colorize.hpp"
#include "../../lexer/tokens.hpp"
#include "../../common.hpp"

namespace Zura {
    class ParserError {
    private:
    public:
        static void errorAt(Token* token, std::string message) {
            // [line -> 1] [pos -> 3] Parser Error 
            std::cout << "[" << termcolor::green  << "line: " << termcolor::yellow << token->line << termcolor::reset << 
                        "] ["<< termcolor::green  << "pos: " << termcolor::yellow << token->column << termcolor::reset <<
                        "] "<< termcolor::magenta << termcolor::italic <<"Parser Error " << termcolor::reset;
            
            // at 'x': Expected ';' after expression
            if (token->kind == EOF_TOKEN) std::cout << "at end";
            else if (token->kind == ERROR_TOKEN); // Do Nothing
            else {
                std::cout << "at '" << termcolor::red << 
                            std::string(token->start, token->length) << 
                            termcolor::reset << "'";
            }
            std::cout << "\n" << termcolor::reset << message << std::endl;

            // Iterat over the tokens in the current line
            // and print out the error line
            // have x := 45
            const char* line_start = get_source_line_start(token->line);
            const char* line_end = line_start;
            while (*line_end != '\n' && *line_end != '\0') line_end++;
            std::cout << termcolor::cyan << std::string(line_start - 1, line_end - line_start) 
                      << termcolor::reset   << std::endl;

            //Print the carrot to tell where the error is at
            //        --------
            int num_spaces = token->column - 2;
            // Highlight from the error token to the end of the line
            std::cout << termcolor::red << std::string(num_spaces, ' ') << "^";
            for (int i = 0; i < token->length; i++) std::cout << "~";
            std::cout << termcolor::reset << std::endl;
        }
    };
}
