#include <iostream>

#include "../lexer/tokens.hpp"
#include "../common.hpp"
#include "compiler.hpp"

void compile(const char *source) {
    init_tokenizer(source);

    int line = -1;

    while (true) {
        Token token = scan_token();
        if (token.line != line) {
            std::cout << token.line << " ";
            line = token.line;
        } else {
            std::cout << "   | ";
        }
        printf("%2d '%.*s'\n", token.kind, token.length, token.start);

        if (token.kind == EOF_TOKEN) break;
    }
}