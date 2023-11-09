#pragma once

#include <string>
#include <unordered_map>

#include "tokens.hpp"

typedef struct {
    const char* source;
    const char* start;
    const char* current;
    int         line;
    int         column;
} Scanner;

Scanner scanner;

void init_tokenizer(const char* source)
{
    scanner.source  = source;
    scanner.start   = source;
    scanner.current = source;
    scanner.line    = 1;
    scanner.column  = 0;
}

const char* get_source_line_start(int line)
{
    const char* current      = scanner.source;
    int         current_line = 1;

    while (current_line < line && *current != '\0') {
        if (*current == '\n')
            current_line++;
        current++;
    }

    // Include leading whitespace
    while (*current != '\n' && *current != '\0' && isspace(*current))
        current--;
    if (*current++ == '\n')
        current++;
    return current;
}

static bool is_alpha(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}
static bool is_digit(char c) { return c >= '0' && c <= '9'; }
static bool is_at_end() { return *scanner.current == '\0'; }

static char advance()
{
    scanner.current++;
    scanner.column++;
    return scanner.current[-1];
}

static char peek_next() { return scanner.current[1]; }
static char peek() { return *scanner.current; }

static bool match(char expected)
{
    if (is_at_end())
        return false;
    if (*scanner.current != expected)
        return false;

    scanner.current++;
    scanner.column++;
    return true;
}

static Token make_token(TokenKind kind)
{
    Token token;
    token.kind   = kind;
    token.line   = scanner.line;
    token.start  = scanner.start;
    token.column = scanner.column;
    token.length = static_cast<int>(scanner.current - scanner.start);
    return token;
}

static Token error_token(std::string message)
{
    Token token;
    token.kind   = ERROR_TOKEN;
    token.start  = message.c_str();
    token.length = message.length();
    token.line   = scanner.line;
    return token;
}

static void skip_whitespace()
{
    for (;;) {
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
            if (peek_next() == '/')
                while (peek() != '\n' && !is_at_end())
                    advance();
            else
                return;
            break;
        default:
            return;
        }
    }
}

static Token _string()
{
    while ((peek() != '"') && !is_at_end()) {
        if (peek() == '\n')
            scanner.line++;
        advance();
    }
    if (is_at_end())
        error_token("Unterminated string.");
    advance(); // advance past the closing '"'
    return make_token(STRING);
}

struct Keyword {
    std::string keyword;
    TokenKind   kind;
};

static TokenKind perfect_hash_lookup(std::string keyword)
{
    static const std::unordered_map<std::string, TokenKind> keyword_hash {
        { "class", CLASS },
        { "else", ELSE },
        { "false", TK_FALSE },
        { "for", FOR },
        { "fn", FN },
        { "if", IF },
        { "info", INFO },
        { "input", TK_INPUT },
        { "return", RETURN },
        { "super", SUPER },
        { "this", TK_THIS },
        { "true", TK_TRUE },
        { "have", HAVE },
        { "static", STATIC },
        { "loop", LOOP },
        { "include", INCLUDE },
        { "sleep", SLEEP },
        { "exit", EXIT },
        { "continue", CONTINUE },
        { "extends", EXTENDS },
        { "break", BREAK },
        { "match", MATCH },
        { "case", CASE },
        { "default", DEFAULT },

        // types
        { "int", TK_INT },
        { "float", TK_FLOAT },
        { "string", STRING_TYPE },
        { "bool", TK_BOOL },
        { "any", ANY },
        { "nil", NIL },
        { "enum", ENUM },
        { "struct", STRUCT }
    };

    auto it = keyword_hash.find(keyword);
    if (it != keyword_hash.end())
        return it->second;
    return IDENTIFIER;
}

static TokenKind identifier_type()
{
    std::string keyword(scanner.start, scanner.current);
    return perfect_hash_lookup(keyword.c_str());
}

static Token identifier()
{
    while (is_alpha(peek()) || is_digit(peek()))
        advance();
    return make_token(identifier_type());
}

static Token number()
{
    while (is_digit(peek()))
        advance();

    // Look for a fractional part.
    if (peek() == '.' && is_digit(peek_next())) {
        advance(); // Consume the "."

        while (is_digit(peek()))
            advance();
    }
    // print out the number
    return make_token(NUMBER);
}
