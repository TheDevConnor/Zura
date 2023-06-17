#ifndef AZURA_LEXER_H
#define AZURA_LEXER_H

#include <string>

#include "../helper/lexer_error.h"
#include "../helper/import.h"
#include "tokens.h"
#include "helper.h"

static Token string() {
    std::string lexeme = "";
    TokenKind type = STRING;
    for(;;) {
        if (peek() == '"') break;
        if (is_at_end()) error_function("Unterminated string.", RED, scanner.start);
        if (peek() == '\n') scanner.line++;
        if (peek() == '$') {
            if (peek_next() == '{') {
                advance(); // advance past the '$'
                advance(); // advance past the '{'

                while (peek() != '}') {
                    if (is_at_end()) error_function("Missing '}' at the end: ", RED, scanner.start);
                    if (peek() == '\n') scanner.line++;
                    lexeme += peek();
                    advance();
                }
                advance(); // advance past the '}'
                type = STRING_INTERPOLATION;
            }
            else error_function("missing a '{' after '$': ", RED, scanner.start);
        }
        lexeme += peek();
        advance();
    }
    advance(); // advance past the closing '"'
    return make_token(type, lexeme);
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