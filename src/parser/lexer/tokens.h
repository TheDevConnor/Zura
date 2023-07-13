#ifndef AZURA_TOKENS_H
#define AZURA_TOKENS_H

using namespace std;

typedef enum {
    // Single-character Tokens!
    LEFT_PAREN, RIGHT_PAREN,     // ( )
    LEFT_BRACE, RIGHT_BRACE,     // { }
    LEFT_BRACKET, RIGHT_BRACKET, // [ ]
    COMMA, DOT, MINUS, PLUS,     // , . - +
    SEMICOLON, SLASH, STAR,      // ; / *
    HASHTAG, COLON, MODULO,      // # : %
    DOLLAR,  POWER,              // $ ^

    // One or two character tokens!
    BANG, BANG_EQUAL,        // ! !=
    EQUAL, EQUAL_EQUAL,      // = ==
    GREATER, GREATER_EQUAL,  // > >=
    LESS, LESS_EQUAL,        // < <=
    WALRUS, INHERITANCE,     // := extends

    // Literals!
    IDENTIFIER, STRING, NUMBER, STRING_INTERPOLATION,

    // Keywords!
    AND, CLASS, ELSE, FALSE, FUNC, FOR, IF, NIL, OR,
    INFO, RETURN, SUPER, THIS, TRUE, HAVE, WHILE, MODULE,
    USING, CONTINUE, BREAK, BIT_AND, BIT_OR,
    
    ERROR_TOKEN,
    // End of file!
    EOF_TOKEN
} TokenKind;

typedef struct {
    TokenKind kind;
    const char* start;
    int length;
    int line;
    int column;
} Token;

void init_tokenizer(const char* source);
const char* get_source_line_start(int line);
Token scan_token();

#endif // AZURA_TOKENS_H