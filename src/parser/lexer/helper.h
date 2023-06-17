#ifndef AZURA_LEXER_HELPER_H
#define AZURA_LEXER_HELPER_H

#include "../helper/import.h"
#include "tokens.h"

typedef struct {
    const char* source;
    const char* start;
    const char* current;
    int line;
    int column;
} Scanner;

Scanner scanner;

void init_tokenizer(const char* source) {
    scanner.source = source;
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

static bool is_alpha(char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';   }
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
    token.length = static_cast<int>(scanner.current - scanner.start);
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
            case '/':
                if (peek_next() == '/') while (peek() != '\n' && !is_at_end()) advance();
                else if (peek_next() == '*') {
                    advance();
                    advance();
                    while (peek() != '*' && peek_next() != '/' && !is_at_end()) advance();
                    advance();
                    advance();
                } else return;
                break;
            default: return;
        }
    }
}

struct Keyword {
    const char* keyword;
    TokenKind kind;
};

static TokenKind perfect_hash_lookup(const char* keyword) {
    // Use the generated perfect hash function to lookup the keyword
    // and return its corresponding token kind.
    // If the keyword is not found, return IDENTIFIER.

    // Example implementation:
    // Hash function: hash(keyword) = index
    // Perfect hash table: perfectHashTable[index] = tokenKind

    // Replace this implementation with your own perfect hash lookup logic.
    // You can use gperf or implement your own perfect hash function generator.
    // The generated code should provide the perfect hash lookup function.

    if (strcmp(keyword, "and")     == 0)     return AND;
    if (strcmp(keyword, "class")   == 0)   return CLASS;
    if (strcmp(keyword, "else")    == 0)    return ELSE;
    if (strcmp(keyword, "false")   == 0)   return FALSE;
    if (strcmp(keyword, "for")     == 0)     return FOR;
    if (strcmp(keyword, "fun")     == 0)     return FUNC;
    if (strcmp(keyword, "if")      == 0)      return IF;
    if (strcmp(keyword, "info")    == 0)    return INFO;
    if (strcmp(keyword, "nil")     == 0)     return NIL;
    if (strcmp(keyword, "or")      == 0)      return OR;
    if (strcmp(keyword, "return")  == 0)  return RETURN;
    if (strcmp(keyword, "super")   == 0)   return SUPER;
    if (strcmp(keyword, "this")    == 0)    return THIS;
    if (strcmp(keyword, "true")    == 0)    return TRUE;
    if (strcmp(keyword, "have")    == 0)    return HAVE;
    if (strcmp(keyword, "while")   == 0)   return WHILE;

    return IDENTIFIER;
}

static TokenKind identifier_type() {
    std::string keyword(scanner.start, scanner.current);
    return perfect_hash_lookup(keyword.c_str());
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

#endif // AZURA_LEXER_HELPER_H