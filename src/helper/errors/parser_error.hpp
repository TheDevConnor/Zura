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
            std::cout << "[line -> " << termcolor::yellow << token->line << termcolor::reset << 
                        "] [pos -> " << termcolor::yellow << token->column << termcolor::reset <<
                        "] error ";
            
            if (token->kind == EOF_TOKEN) {
                std::cout << "at end";
            } else if (token->kind == ERROR_TOKEN) {
                // Nothing.
            } else {
                std::cout << "at '" << termcolor::red << 
                            std::string(token->start, token->length) << 
                            termcolor::reset << "'";
            }

            std::cout << ": " << termcolor::reset << message << std::endl;
            Zura::Exit(Zura::Exit_Value::PARSER_ERROR);
        }
    };
}
