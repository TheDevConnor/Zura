#pragma once

// #include "../../terminal_colors/terminal_color.h"
#include "../../lib/colorize.hpp"
#include "../../compiler/object.h"
#include "../lexer/lexer.h"
#include "../../common.h"

using namespace std;

class Parser {
public:
    Token current;
    Token previous;
    bool had_error;
    bool panic_mode;

    Parser() : had_error(false), panic_mode(false) {}

    void error_parser(Token* token, const char* message) {
        if(panic_mode) return;
        panic_mode = true;

        cout << "\n[" << termcolor::yellow << "line: " << termcolor::cyan << token->line << termcolor::reset << "] " <<
                  "[" << termcolor::yellow << "pos: "  << termcolor::cyan << token->column - 1 << termcolor::reset << "] Error: ";

        if (token->kind == EOF_TOKEN) printf("at end \n");
        else if (token->kind == ERROR_TOKEN) {} // Nothing
        else cout << "at " << termcolor::red << "'" << termcolor::reset << termcolor::cyan << token->length << 
                              termcolor::reset << termcolor::red << "'" << termcolor::reset << "\n";

        // iterator over the tokens in the current line
        const char* line_start = get_source_line_start(token->line);
        const char* line_end = line_start;
        while (*line_end != '\n' && *line_end != '\0') line_end++;

        // Print the line
        cout << termcolor::magenta << string(line_start - 1, line_end - line_start) << termcolor::reset << "\n";
        
        // Print to the error
        int num_spaces = token->column - 2;
        for (int i = 0; i < num_spaces; i++) cout << " ";
        cout << termcolor::red << "^" << termcolor::reset << " ";

        // Print the error message
        cout << message << "\n";
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

struct Local {
    Token name;
    int depth;
    bool is_captured;
};

enum FunctionType {
    TYPE_FUNCTION,
    TYPE_INITIALIZER,
    TYPE_METHOD,
    TYPE_SCRIPT,
};

struct Upvalue {
    uint8_t index;
    bool is_local;
};

struct Compiler {
    struct Compiler* enclosing;
    ObjFunction* function;
    FunctionType type;

    Local locals[UINT8_COUNT];
    int local_count;
    Upvalue upvalues[UINT8_COUNT];
    int scope_depth;
};

struct ClassCompiler {
    struct ClassCompiler* enclosing;
    bool has_superclass;
};

struct Parser parser;
struct Compiler* current = nullptr;
ClassCompiler* current_class = nullptr;

void init_compiler(Compiler *compiler, FunctionType type);
ObjFunction *end_compiler();

Chunk* compiling_chunk() { return &current->function->chunk; }

Token synthetic_token(const char* text);
void super_(bool can_assign);

void begin_scope() { current->scope_depth++; }
void end_scope();

uint8_t parser_variable(const char *error_msg);

void add_local(Token name);
void declare_variable();
void mark_initialized();
void define_variable(uint8_t global);

uint8_t argument_list();

void expression();
void statement();
void declaration();

ParseRule *get_rule(TokenKind kind);

void named_variable(Token name, bool can_assign);
uint8_t identifier_constant(Token *name);
bool identifiers_equal(Token *a, Token *b);

void grouping(bool can_assign);
void _number(bool can_assign);
void _string(bool can_assign);
void _variable(bool can_assign);
void _this(bool can_assign);
void and_(bool can_assign);
void or_(bool can_assign);
void unary(bool can_assign);
void call(bool can_assign);
void dot(bool can_assign);
void binary(bool can_assign);
void literal(bool can_assign);
void array_literal(bool can_assign);
void input_statement(bool can_assign);
void parse_precedence(Precedence prec);

void expression() { parse_precedence(PREC_ASSIGNMENT); }

int inner_most_loop_start = -1;
int inner_most_loop_scope_depth = 0;

void emit_byte(uint8_t byte) {
  write_chunk(compiling_chunk(), byte, parser.previous.line);
}

void emit_bytes(uint8_t byte1, uint8_t byte2) {
  emit_byte(byte1);
  emit_byte(byte2);
}

void emit_loop(int loop_start) {
  emit_byte(OP_LOOP);

  int offset = compiling_chunk()->count - loop_start + 2;
  if (offset > UINT16_MAX)
    parser.error("Loop body too large.");

  emit_byte((offset >> 8) & 0xff);
  emit_byte(offset & 0xff);
}

int emit_jump(uint8_t instruction) {
  emit_byte(instruction);
  emit_byte(0xff);
  emit_byte(0xff);
  return compiling_chunk()->count - 2;
}

uint8_t make_constant(Value value) {
  int constant = add_constant(compiling_chunk(), value);
  if (constant > UINT8_MAX) {
    parser.error("Too many constants in one chunk.");
    return 0;
  }

  return static_cast<uint8_t>(constant);
}

void emit_return() {
  if (current->type == TYPE_INITIALIZER)
    emit_bytes(OP_GET_LOCAL, 0);
  else
    emit_byte(OP_NIL);
  emit_byte(OP_RETURN);
}

void emit_constant(Value v) { emit_bytes(OP_CONSTANT, make_constant(v)); }

void patch_jump(int offset) {
  // -2 to adjust for the bytecode for the jump offset itself.
  int jump = compiling_chunk()->count - offset - 2;

  if (jump > UINT16_MAX) {
    parser.error("Too much code to jump over.");
  }

  compiling_chunk()->code[offset] = (jump >> 8) & 0xff;
  compiling_chunk()->code[offset + 1] = jump & 0xff;
}

void var_declaration();

unordered_map<TokenKind, ParseRule> rules = {
    {LEFT_PAREN,    {grouping,     call, PREC_CALL}},
    {RIGHT_PAREN,   {nullptr,   nullptr, PREC_NONE}},
    {LEFT_BRACE,    {nullptr,   nullptr, PREC_NONE}},
    {RIGHT_BRACE,   {nullptr,   nullptr, PREC_NONE}},
    {LEFT_BRACKET,  {array_literal,   nullptr,   PREC_NONE}},
    {RIGHT_BRACKET, {nullptr,   nullptr,  PREC_NONE}},
    {COMMA,         {nullptr,   nullptr,  PREC_NONE}},
    {DOT,           {nullptr,       dot,  PREC_CALL}},
    {MINUS,         {unary,      binary,  PREC_TERM}},
    {PLUS,          {nullptr,    binary,  PREC_TERM}},
    {SEMICOLON,     {nullptr,   nullptr,  PREC_NONE}},
    {SLASH,         {nullptr,    binary,  PREC_FACTOR}},
    {STAR,          {nullptr,    binary,  PREC_FACTOR}},
    {MODULO,        {nullptr,    binary,  PREC_FACTOR}},
    {POWER,         {nullptr,    binary,  PREC_FACTOR}},
    {INCREMENT,     {unary,     nullptr,  PREC_NONE}},
    {DECREMENT,     {unary,     nullptr,  PREC_NONE}},
    {BANG,          {unary,     nullptr,  PREC_NONE}},
    {BANG_EQUAL,    {nullptr,    binary,  PREC_EQUALITY}},
    {EQUAL,         {nullptr,    binary,  PREC_COMPARISON}},
    {EQUAL_EQUAL,   {nullptr,    binary,  PREC_COMPARISON}},
    {GREATER,       {nullptr,    binary,  PREC_COMPARISON}},
    {GREATER_EQUAL, {nullptr,    binary,  PREC_COMPARISON}},
    {LESS,          {nullptr,    binary,  PREC_COMPARISON}},
    {LESS_EQUAL,    {nullptr,    binary,  PREC_COMPARISON}},
    {IDENTIFIER,    {_variable, nullptr,  PREC_NONE}},
    {STRING,        {_string,   nullptr,  PREC_NONE}},
    {NUMBER,        {_number,   nullptr,  PREC_NONE}},
    {AND,           {nullptr,      and_,  PREC_AND}},
    {CLASS,         {nullptr,   nullptr,  PREC_NONE}},
    {ELSE,          {nullptr,   nullptr,  PREC_NONE}},
    {TK_FALSE,      {literal,   nullptr,  PREC_NONE}},
    {FUNC,          {nullptr,   nullptr,  PREC_NONE}},
    {FOR,           {nullptr,   nullptr,  PREC_NONE}},
    {CONTINUE,      {nullptr,   nullptr,  PREC_NONE}},
    {BREAK,         {nullptr,   nullptr,  PREC_NONE}},
    {IF,            {nullptr,   nullptr,  PREC_NONE}},
    {NIL,           {literal,   nullptr,  PREC_NONE}},
    {OR,            {nullptr,       or_,  PREC_OR}},
    {INFO,          {nullptr,   nullptr,  PREC_NONE}},
    {RETURN,        {nullptr,   nullptr,  PREC_NONE}},
    {SUPER,         {super_,    nullptr,  PREC_NONE}},
    {TK_THIS,       {_this,     nullptr,  PREC_NONE}},
    {TK_TRUE,       {literal,   nullptr,  PREC_NONE}},
    {VAR,           {nullptr,   nullptr,  PREC_NONE}},
    {WHILE,         {nullptr,   nullptr,  PREC_NONE}},
    {MODULE,        {nullptr,   nullptr,  PREC_NONE}},
    {INCLUDE,       {nullptr,   nullptr,  PREC_NONE}},
    {INAPPEND,      {nullptr,   nullptr,  PREC_NONE}},
    {TK_INPUT,      {input_statement,   nullptr,  PREC_NONE}},
    {ERROR_TOKEN,   {nullptr,   nullptr,  PREC_NONE}},
    {EOF_TOKEN,     {nullptr,   nullptr,  PREC_NONE}},
};

const unordered_map<TokenKind, bool> return_context = {
    {CLASS, true},
    {FUNC, true},
    {VAR, true},
    {FOR, true},
    {IF, true},
    {WHILE, true},
    {INFO, true},
    {RETURN, true}
};
