#ifndef azura_helper_h
#define azura_helper_h

#include <cctype>
#include <string>
#include <unordered_map>

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

static TokenKind identifier_type() {
    std::unordered_map<std::string, TokenKind> keywordMap = {
        {"and", AND},
        {"class", CLASS},
        {"else", ELSE},
        {"false", FALSE},
        {"for", FOR},
        {"fun", FUNC},
        {"if", IF},
        {"info", INFO},
        {"nil", NIL},
        {"or", OR},
        {"return", RETURN},
        {"super", SUPER},
        {"this", THIS},
        {"true", TRUE},
        {"have", HAVE},
        {"while", WHILE}
    };

    std::string keyword(scanner.start, scanner.current);
    auto it = keywordMap.find(keyword);
    if(it != keywordMap.end()) return it->second;
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

        while (is_digit(peek())) advance();
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