#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "parser.h"
#include "lexer/lexer.h"

class Parser {
private:
    typedef enum {
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
        RESET
    } Color;

    const char* colorize(Color color) {
        switch (color) {
            case RED: return "\033[0;31m";
            case GREEN: return "\033[0;32m";
            case YELLOW: return "\033[0;33m";
            case BLUE: return "\033[0;34m";
            case MAGENTA: return "\033[0;35m";
            case CYAN: return "\033[0;36m";
            case WHITE: return "\033[0;37m";
            case RESET: return "\033[0m";
            default: return "\033[0m";
        }
    }
public:
    Token current;
    Token previous;
    bool had_error;
    bool panic_mode;

    Parser() : had_error(false), panic_mode(false) {}

    void error_parser(Token* token, const char* message) {
        had_error = true;
        if(panic_mode) return;
        panic_mode = true;

        std::printf("\n[%sline: %s%d%s -- %spos: %s%d%s] Error: ", 
            colorize(YELLOW), colorize(CYAN), token->line, colorize(YELLOW), 
            colorize(YELLOW), colorize(CYAN), token->column - 1, colorize(RESET));

        if (token->kind == EOF_TOKEN) std::printf("at end \n");
        else if (token->kind == ERROR_TOKEN) {}
        else std::printf("at %s'%.*s'%s\n", colorize(RED), token->length, token->start, colorize(RESET));

        // iterator over the tokens in the current line
        const char* line_start = get_source_line_start(token->line);
        const char* line_end = line_start;
        while (*line_end != '\n' && *line_end != '\0') line_end++;

        // Print the line
        std::fprintf(stderr, "%s%.*s\n", colorize(MAGENTA), (int)(line_end - line_start), line_start);

        // Print to the error
        int num_spaces = token->column - 1;
        for (int i = 0; i < num_spaces; i++) std::cout << " ";
        std::cout << colorize(RED) << "^" << colorize(RESET);

        // Print the error message
        std::cout << message;
    }

    void error_at_current(const char* message) { error_parser(&current, message); }
    void error(const char* message) { error_parser(&previous, message); }

    void advance() {
        previous = scan_token();
        while (current.kind == ERROR_TOKEN) {
            current = scan_token();
            if (current.kind == ERROR_TOKEN) {
                error_at_current(previous.start);
            }
        }
    }

    void consume(TokenKind kind, const char* message) {
        if (current.kind == kind) {
            advance();
            return;
        }

        error_at_current(message);
    }
};

typedef void (*parse_fn)();

enum Precedence {
    PREC_NONE,
    PREC_ASSIGNMENT,  // :=
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
};

struct ParseRule {
    parse_fn prefix;
    parse_fn infix;
    Precedence precedence;
};


Parser parser;
Chunk* compiling_chunk;

void emit_byte(uint8_t byte) {
    write_chunk(compiling_chunk, byte, parser.previous.line);
}

void emit_bytes(uint8_t byte1, uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
}

uint8_t make_constant(Value value) {
    int constant = add_constant(compiling_chunk, value);
    if (constant > UINT8_MAX) {
        parser.error("Too many constants in one chunk.");
        return 0;
    }

    return static_cast<uint8_t>(constant);
}

void emit_return() {
    emit_byte(OP_RETURN);
}

void emit_constant(Value v) {
    emit_bytes(OP_CONSTANT, make_constant(v));
}

void end_compiler() {
    emit_return();
}

ParseRule* get_rule(TokenKind kind);
void parse_precedence(Precedence precedence);

void binary() {
    // Remember the operator
    TokenKind operator_type = parser.previous.kind;

    // Compile the right operand
    ParseRule* rule = get_rule(operator_type);
    parse_precedence(static_cast<Precedence>(rule->precedence + 1));

    // Emit the operator instruction
    switch (operator_type) {
        case PLUS:   emit_byte(OP_ADD);      break;
        case MINUS:  emit_byte(OP_SUBTRACT); break;
        case STAR:   emit_byte(OP_MULTIPLY); break;
        case SLASH:  emit_byte(OP_DIVIDE);   break;
        case MODULO: emit_byte(OP_MODULO);   break;
        default: return;
    }
}

void grouping() {
    parse_precedence(PREC_ASSIGNMENT);
    parser.consume(RIGHT_PAREN, "Expect ')' after expression.");
}

void _number() {
    double value = std::strtod(parser.previous.start, nullptr);
    emit_constant(value);
}

void unary() {
    TokenKind operator_type = parser.previous.kind;

    // Compile the operand
    parse_precedence(PREC_UNARY);

    // Emit the operator instruction
    switch (operator_type) {
        case MINUS:
            emit_byte(OP_NEGATE);
            break;
        default:
            return;
    }
}

std::unordered_map<TokenKind, ParseRule> rules = {
    {LEFT_PAREN,    {grouping, nullptr,   PREC_NONE}},
    {RIGHT_PAREN,   {nullptr,   nullptr,   PREC_NONE}},
    {LEFT_BRACE,    {nullptr,   nullptr,   PREC_NONE}},
    {RIGHT_BRACE,   {nullptr,   nullptr,   PREC_NONE}},
    {COMMA,         {nullptr,   nullptr,   PREC_NONE}},
    {DOT,           {nullptr,   nullptr,   PREC_NONE}},
    {MINUS,         {unary,     binary,     PREC_TERM}},
    {PLUS,          {nullptr,   binary,     PREC_TERM}},
    {SEMICOLON,     {nullptr,   nullptr,   PREC_NONE}},
    {SLASH,         {nullptr,   binary,     PREC_FACTOR}},
    {STAR,          {nullptr,   binary,     PREC_FACTOR}},
    {BANG,          {nullptr,   nullptr,   PREC_NONE}},
    {BANG_EQUAL,    {nullptr,   nullptr,   PREC_NONE}},
    {EQUAL,         {nullptr,   nullptr,   PREC_NONE}},
    {EQUAL_EQUAL,   {nullptr,   nullptr,   PREC_NONE}},
    {GREATER,       {nullptr,   nullptr,   PREC_NONE}},
    {GREATER_EQUAL, {nullptr,   nullptr,   PREC_NONE}},
    {LESS,          {nullptr,   nullptr,   PREC_NONE}},
    {LESS_EQUAL,    {nullptr,   nullptr,   PREC_NONE}},
    {IDENTIFIER,    {nullptr,   nullptr,   PREC_NONE}},
    {STRING,        {nullptr,   nullptr,   PREC_NONE}},
    {NUMBER,        {_number,    nullptr,   PREC_NONE}},
    {AND,           {nullptr,   nullptr,   PREC_NONE}},
    {CLASS,         {nullptr,   nullptr,   PREC_NONE}},
    {ELSE,          {nullptr,   nullptr,   PREC_NONE}},
    {FALSE,         {nullptr,   nullptr,   PREC_NONE}},
    {FUNC,          {nullptr,   nullptr,   PREC_NONE}},
    {FOR,           {nullptr,   nullptr,   PREC_NONE}},
    {IF,            {nullptr,   nullptr,   PREC_NONE}},
    {NIL,           {nullptr,   nullptr,   PREC_NONE}},
    {OR,            {nullptr,   nullptr,   PREC_NONE}},
    {INFO,          {nullptr,   nullptr,   PREC_NONE}},
    {RETURN,        {nullptr,   nullptr,   PREC_NONE}},
    {SUPER,         {nullptr,   nullptr,   PREC_NONE}},
    {THIS,          {nullptr,   nullptr,   PREC_NONE}},
    {TRUE,          {nullptr,   nullptr,   PREC_NONE}},
    {HAVE,          {nullptr,   nullptr,   PREC_NONE}},
    {WHILE,         {nullptr,   nullptr,   PREC_NONE}},
    {ERROR_TOKEN,   {nullptr,   nullptr,   PREC_NONE}},
    {EOF_TOKEN,     {nullptr,   nullptr,   PREC_NONE}},
};

void parse_precedence(Precedence prec) {
    parser.advance(); // Consume the operator
    parse_fn prefix_rule = get_rule(parser.previous.kind)->prefix;
    if (prefix_rule == nullptr) {
        parser.error("Expect expression.");
        return;
    }

    prefix_rule();

    while (prec <= get_rule(parser.current.kind)->precedence) {
        parser.advance();
        parse_fn infix_rule = get_rule(parser.previous.kind)->infix;
        infix_rule();
    }
}

ParseRule* get_rule(TokenKind kind) {
    return &rules[kind];
}

bool compile(const char* source, Chunk* chunk) {
    init_tokenizer(source);
    compiling_chunk = chunk;

    parser.had_error = false;
    parser.panic_mode = false;

    parser.advance();
    parse_precedence(PREC_ASSIGNMENT);

    parser.consume(EOF_TOKEN, "Expect end of expression.");
    end_compiler();
    return !parser.had_error;
}
