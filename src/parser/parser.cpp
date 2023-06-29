#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "lexer/lexer.h"
#include "common.h"
#include "parser.h"

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
        if(panic_mode) return;
        panic_mode = true;

        std::printf("\n[%sline: %s%d%s] [%spos: %s%d%s] Error: ", 
        colorize(YELLOW), colorize(CYAN), token->line, colorize(RESET), 
            colorize(YELLOW), colorize(CYAN), token->column - 1, colorize(RESET));

        if (token->kind == EOF_TOKEN) std::printf("at end \n");
        else if (token->kind == ERROR_TOKEN) {}
        else std::printf("at %s'%.*s'%s\n", colorize(RED), token->length, token->start, colorize(RESET));

        // iterator over the tokens in the current line
        const char* line_start = token->start;
        const char* line_end = line_start;
        while (*line_end != '\n' && *line_end != '\0') line_end++;

        // Print the line
        std::cout << colorize(MAGENTA) << std::string(line_start, line_end - line_start) << "\n";

        // Print to the error
        int num_spaces = token->column - 1;
        for (int i = 0; i < num_spaces; i++) std::cout << " ";
        std::cout << colorize(RED) << "^" << colorize(RESET);

        // Print the error message
        std::cout << message << "\n";
        had_error = true;
    }

    void error_at_current(const char* message) { error_parser(&current, message); }
    void error(const char* message) { error_parser(&previous, message); }

    bool check(TokenKind kind) { return current.kind == kind; }

    void advance() {
        previous = current;
        for(;;) {
            current = scan_token();
            if(current.kind != ERROR_TOKEN) break;
            error_at_current(current.start);
        }
    }

    void consume(TokenKind kind, const char* message) {
        if (current.kind == kind) {
            advance();
            return;
        }

        error_at_current(message);
    }

    bool match(TokenKind kind) {
        if (!check(kind)) return false;
        advance();
        return true;
    }
};

typedef void (*parse_fn)(bool can_assign);

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
    #ifndef DEBUG_PRINT_CODE
        #include "../debug/debug.h"
        if (parser.had_error) disassemble_chunk(compiling_chunk, "code");
    #endif
}

void expression();
void statement();
void declaration();
ParseRule* get_rule(TokenKind kind);
void parse_precedence(Precedence precedence);

uint8_t identifier_constant(Token* name) {
    return make_constant(OBJ_VAL(copy_string(name->start, name->length)));
}

uint8_t parser_variable(const char* error_msg) {
    parser.consume(IDENTIFIER, error_msg);
    return identifier_constant(&parser.previous);
}

void define_variable(uint8_t global) {
    emit_bytes(OP_DEFINE_GLOBAL, global);
}

void binary(bool can_assign) {
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
        case POWER:   emit_byte(OP_POWER);   break;

        // Comparison operators
        case BANG_EQUAL:    emit_bytes(OP_EQUAL, OP_NOT);   break; // !=
        case EQUAL_EQUAL:   emit_byte(OP_EQUAL);            break; // ==
        case GREATER:       emit_byte(OP_GREATER);          break; // >
        case GREATER_EQUAL: emit_bytes(OP_LESS, OP_NOT);    break; // >=
        case LESS:          emit_byte(OP_LESS);             break; // <
        case LESS_EQUAL:    emit_bytes(OP_GREATER, OP_NOT); break; // <=

        default: return; // Unreachable
    }
}

void literal(bool can_assign) {
    switch (parser.previous.kind) {
        case FALSE: emit_byte(OP_FALSE); break;
        case TRUE:  emit_byte(OP_TRUE);  break;
        case NIL:   emit_byte(OP_NIL);   break;
        default: return; // Unreachable
    }
}

void expression() { parse_precedence(PREC_ASSIGNMENT); }

void var_declaration() {
    uint8_t global = parser_variable("Expect variable name.");

    if (parser.match(WALRUS)) expression();
    else emit_byte(OP_NIL);

    parser.consume(SEMICOLON, "Expect ';' after variable declaration.");
    define_variable(global);
}

void expression_statement() {
    expression();
    parser.consume(SEMICOLON, "Expect ';' after expression.");
    emit_byte(OP_POP);
}

void info_statement() {
    expression();
    parser.consume(SEMICOLON, "Expect ';' after value.");
    emit_byte(OP_INFO);
}

const std::unordered_map<TokenKind, bool> return_context = {
    {CLASS, true},
    {FUNC, true},
    {HAVE, true},
    {FOR, true},
    {IF, true},
    {WHILE, true},
    {INFO, true},
    {RETURN, true}
};

void synchronize() {
    parser.panic_mode = false;

    while (parser.current.kind != EOF_TOKEN) {
        if (parser.previous.kind == SEMICOLON) return;
        if (return_context.find(parser.current.kind) != return_context.end()) return;
        parser.advance();
    }
}

void declaration() {
    if (parser.match(HAVE)) { var_declaration(); }
    else { statement(); }
    if (parser.panic_mode) synchronize();
}

void statement() {
    if (parser.match(INFO)) { info_statement(); }
    else { expression_statement(); }
}

void grouping(bool can_assign) {
    expression();
    parser.consume(RIGHT_PAREN, "Expect ')' after expression.");
}

void named_variable(Token name, bool can_assign) {
    uint8_t arg = identifier_constant(&name);
    
    if (parser.match(EQUAL) && can_assign) {
        expression();
        emit_bytes(OP_SET_GLOBAL, arg);
    } else {
        emit_bytes(OP_GET_GLOBAL, arg);
    }
}

void _number(bool can_assign) {
    double value = std::strtod(parser.previous.start, nullptr);
    emit_constant(NUMBER_VAL(value));
}

void _string(bool can_assign) {
    emit_constant(OBJ_VAL(copy_string(parser.previous.start + 1, parser.previous.length - 2)));
}

void _variable(bool can_assign) {
    named_variable(parser.previous, can_assign);
}

void unary(bool can_assign) {
    TokenKind operator_type = parser.previous.kind;

    // Compile the operand
    parse_precedence(PREC_UNARY);

    // Emit the operator instruction
    switch (operator_type) {
        case BANG:  emit_byte(OP_NOT); break;
        case MINUS: emit_byte(OP_NEGATE); break;
        default: return; // Unreachable
    }
}

std::unordered_map<TokenKind, ParseRule> rules = {
    {LEFT_PAREN,    {grouping,  nullptr,   PREC_NONE}},
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
    {MODULO,        {nullptr,   binary,     PREC_FACTOR}},
    {POWER,         {nullptr,   binary,     PREC_FACTOR}},
    {BANG,          {unary,     nullptr,   PREC_NONE}},
    {BANG_EQUAL,    {nullptr,   binary,     PREC_EQUALITY}},
    {EQUAL,         {nullptr,   binary,     PREC_COMPARISON}},
    {EQUAL_EQUAL,   {nullptr,   binary,     PREC_COMPARISON}},
    {GREATER,       {nullptr,   binary,     PREC_COMPARISON}},
    {GREATER_EQUAL, {nullptr,   binary,     PREC_COMPARISON}},
    {LESS,          {nullptr,   binary,     PREC_COMPARISON}},
    {LESS_EQUAL,    {nullptr,   binary,     PREC_COMPARISON}},
    {IDENTIFIER,    {_variable,  nullptr,   PREC_NONE}},
    {STRING,        {_string,    nullptr,   PREC_NONE}},
    {NUMBER,        {_number,    nullptr,   PREC_NONE}},
    {AND,           {nullptr,   nullptr,   PREC_NONE}},
    {CLASS,         {nullptr,   nullptr,   PREC_NONE}},
    {ELSE,          {nullptr,   nullptr,   PREC_NONE}},
    {FALSE,         {literal,   nullptr,   PREC_NONE}},
    {FUNC,          {nullptr,   nullptr,   PREC_NONE}},
    {FOR,           {nullptr,   nullptr,   PREC_NONE}},
    {IF,            {nullptr,   nullptr,   PREC_NONE}},
    {NIL,           {literal,   nullptr,   PREC_NONE}},
    {OR,            {nullptr,   nullptr,   PREC_NONE}},
    {INFO,          {nullptr,   nullptr,   PREC_NONE}},
    {RETURN,        {nullptr,   nullptr,   PREC_NONE}},
    {SUPER,         {nullptr,   nullptr,   PREC_NONE}},
    {THIS,          {nullptr,   nullptr,   PREC_NONE}},
    {TRUE,          {literal,   nullptr,   PREC_NONE}},
    {HAVE,          {nullptr,   nullptr,   PREC_NONE}},
    {WHILE,         {nullptr,   nullptr,   PREC_NONE}},
    {MODULE,        {nullptr,   nullptr,   PREC_NONE}},
    {USING,         {nullptr,   nullptr,   PREC_NONE}},
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

    bool can_assign = prec <= PREC_ASSIGNMENT;
    prefix_rule(can_assign);

    while (prec <= get_rule(parser.current.kind)->precedence) {
        parser.advance();
        parse_fn infix_rule = get_rule(parser.previous.kind)->infix;
        infix_rule(can_assign);
    }

    if (can_assign && match(EQUAL)) {
        parser.error("Invalid assignment target.");
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

    while (!parser.match(EOF_TOKEN)) {
        declaration();
    }

    end_compiler();
    return !parser.had_error;
}
