#ifndef azura_lexer_h
#define azura_lexer_h

#include <cstdio>
#include <cstring>
#include <cctype>

#include "tokens.hpp"
#include "helper.hpp"

Token scan_token() {
    skip_whitespace();
    scanner.start = scanner.current;

    if(is_at_end()) return make_token(EOF_TOKEN);

    char c = advance();

    if(isdigit(c)) return number();
    if(isalpha(c)) return identifier();

    switch (c) {
        case ')': return make_token(RIGHT_PAREN);
        case '(': return make_token(LEFT_PAREN);
        case '{': return make_token(LEFT_BRACE);
        case '}': return make_token(RIGHT_BRACE);
        case ';': return make_token(SEMICOLON);
        case ',': return make_token(COMMA);
        case '.': return make_token(DOT);
        case '+': return make_token(PLUS);
        case '/': return make_token(SLASH);
        case '*': return make_token(STAR);
        case '#': return make_token(HASHTAG);
        case '-': return make_token(match('>') ? INHERITANCE   : MINUS);
        case '!': return make_token(match('=') ? BANG_EQUAL    : BANG);
        case '=': return make_token(match('=') ? EQUAL_EQUAL   : EQUAL);
        case ':': return make_token(match('=') ? WALRUS        : COLON);
        case '>': return make_token(match('=') ? GREATER_EQUAL : GREATER);
        case '<': return make_token(match('=') ? LESS_EQUAL    : LESS);
        case '"': return string();
    }
    std::fprintf(stderr, "Unexpected character at line -> ");
    return make_token(ERROR_TOKEN);
}

#endif