#ifndef azura_helper_h
#define azura_helper_h

#include <ctype.h>
#include <string.h>

#include "tokens.hpp"

typedef struct {
    const char* start;
    const char* current;
    int line;
    int column;
} Scanner;

Scanner scanner;

void init_tokenizer(const char* source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
    scanner.column = 0;
}

const char* get_source_line_start(int line) {
    const char* current = scanner.start;
    int current_line = 1;

    while (current_line < line) {
        if (*current == '\n') current_line++;
        current++;
    }

    // Include leading whitespace
    while (*current != '\n' && isspace(*current)) current--;
    if (*current++ == '\n') current++;
    return current;
}

static bool is_alpha(char c) { return isalpha(c) || c == '_';   }
static bool is_digit(char c) { return c >= '0' && c <= '9';     }
static bool is_at_end()      { return *scanner.current == '\0'; }

static char advance() {
    scanner.current++;
    scanner.column++;
    return scanner.current[-1];
}
static char peek_next()     { return scanner.current[1]; }
static char peek()          { return *scanner.current;   }

static bool match(char expected) {
    if (is_at_end()) return false;
    if (*scanner.current != expected) return false;

    scanner.current++;
    scanner.column++;
    return true;
}

static Token make_token(TokenKind kind) {
    Token token;
    token.kind = kind;
    token.line = scanner.line;
    token.start = scanner.start;
    token.column = scanner.column;
    token.length = (int)(scanner.current - scanner.start);
    return token;
}

static Token error_token(const char* message) {
    Token token;
    token.kind = ERROR_TOKEN;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    token.column = scanner.column;
    return token;
}

static TokenKind check_keyword(int start, int length, const char* rest, TokenKind kind) {
    if (scanner.current - scanner.start == start + length &&
        memcmp(scanner.start + start, rest, length) == 0) {
            return kind;
        }
    return IDENTIFIER;
}

static void skip_whitespace() {
    for(;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                scanner.line++;
                scanner.column = 0;
                advance();
                break;
            case '#':
                while (peek() != '\n' && !is_at_end()) advance();
                break;
            default:
                return;
        }
    }
}

// TODO: MAKE THIS NICER!
static TokenKind identifier_type() {
    switch (scanner.start[0]) {
        case 'a': return check_keyword(1, 2, "nd", AND);
        case 'c': return check_keyword(1, 4, "lass", CLASS);
        case 'e': return check_keyword(1, 3, "lse", ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a': return check_keyword(2, 3, "lse", FALSE);
                    case 'o': return check_keyword(2, 1, "r", FOR);
                    case 'u': return check_keyword(2, 1, "n", FUNC);
                }
            }
            break;
        case 'i': 
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'f': return IF;
                    case 'n': return check_keyword(2, 2, "fo", INFO);
                }
            }
            break;
        case 'n': return check_keyword(1, 2, "il", NIL);
        case 'o': return check_keyword(1, 1, "r", OR);
        case 'r': return check_keyword(1, 5, "eturn", RETURN);
        case 's': return check_keyword(1, 4, "uper", SUPER);
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h': return check_keyword(2, 2, "is", THIS);
                    case 'r': return check_keyword(2, 2, "ue", TRUE);
                }
            }
            break;
        case 'h': return check_keyword(1, 3, "ave", HAVE);
        case 'w': return check_keyword(1, 4, "hile", WHILE);
    }
    return IDENTIFIER;
}

static Token identifier() {
    while (is_alpha(peek()) || is_digit(peek())) advance();
    return make_token(identifier_type());
}

static Token number() {
    while (is_digit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && is_digit(peek_next())) {
        advance(); // Consume the "."

        while (is_digit(peek())) advance;
    }
    return make_token(NUMBER);
}

static Token string() {
    while ((peek() != '"') && !is_at_end()) {
        if (peek() == '\n') scanner.line++;
        advance();
    }

    if (is_at_end()) return error_token("Unterminated string.");
    advance(); // The closing ".
    return make_token(STRING);
}

#endif