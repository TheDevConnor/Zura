#include <stdio.h>

#include "common.h"
#include "parser.h"
#include "lexer/lexer.h"

void compile(const char* source) {
    init_tokenizer(source);
    int line = -1;
    for (;;) {
        Token token = scan_token();
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.kind, token.length, token.start);
        if (token.kind == EOF_TOKEN) break;
    }
}