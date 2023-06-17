#ifndef AZURA_LEXER_H
#define AZURA_LEXER_H

#include "../helper/lexer_error.h"
#include "../helper/import.h"
#include "tokens.h"
#include "helper.h"

static Token string() {
    while ((peek() != '"') && !is_at_end()) {
        if (peek() == '\n') scanner.line++;
        advance();
    }

    if (is_at_end()) 
        error_function("Unterminated string.", RED, scanner.start);
    advance(); // The closing ".
    return make_token(STRING);
}

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
        case '[': return make_token(LEFT_BRACKET);
        case ']': return make_token(RIGHT_BRACKET);
        case ';': return make_token(SEMICOLON);
        case ',': return make_token(COMMA);
        case '.': return make_token(DOT);
        case '+': return make_token(PLUS);
        case '/': return make_token(SLASH);
        case '*': return make_token(STAR);
        case '%': return make_token(MODULO);
        case '#': return make_token(HASHTAG);
        case '$': return make_token(match('{') ? STRING_INTERPOLATION : DOLLAR);
        case '-': return make_token(match('>') ? INHERITANCE   : MINUS);
        case '!': return make_token(match('=') ? BANG_EQUAL    : BANG);
        case '=': return make_token(match('=') ? EQUAL_EQUAL   : EQUAL);
        case ':': return make_token(match('=') ? WALRUS        : COLON);
        case '>': return make_token(match('=') ? GREATER_EQUAL : GREATER);
        case '<': return make_token(match('=') ? LESS_EQUAL    : LESS);
        case '"': return string();
    }

    error_function("Unexpected character: ", RED, std::string(1, c));
    return make_token(ERROR_TOKEN);
}

#endif // AZURA_LEXER_H