#include <iostream>
#include <string>

#include "helper/parser_helper.h"
#include "parser.h"

void emit_byte(uint8_t byte) {
    write_chunk(compiling_chunk, byte, parser.previous.line);
}

void emit_bytes(uint8_t byte1, uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
}

int emit_jump(uint8_t instruction) {
    emit_byte(instruction);
    emit_byte(0xff);
    emit_byte(0xff);
    return compiling_chunk->count - 2;
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

void patch_jump(int offset) {
    // -2 to adjust for the bytecode for the jump offset itself.
    int jump = compiling_chunk->count - offset - 2;

    if (jump > UINT16_MAX) {
        parser.error("Too much code to jump over.");
    }

    compiling_chunk->code[offset] = (jump >> 8) & 0xff;
    compiling_chunk->code[offset + 1] = jump & 0xff;
}

void init_compiler(Compiler* compiler) {
    compiler->local_count = 0;
    compiler->scope_depth = 0;
    current = compiler;
}

void end_compiler() {
    emit_return();
    #ifndef DEBUG_PRINT_CODE
        #include "../debug/debug.h"
        if (parser.had_error) disassemble_chunk(compiling_chunk, "code");
    #endif
}

void begin_scope() { current->scope_depth++; }
void end_scope()   { 
    current->scope_depth--; 

    while(current->local_count > 0 && current->locals[current->local_count - 1].depth > current->scope_depth) {
        emit_byte(OP_POP);
        current->local_count--;
    }
}

void expression();
void statement();
void declaration();
ParseRule* get_rule(TokenKind kind);
void parse_precedence(Precedence precedence);

uint8_t identifier_constant(Token* name) {
    return make_constant(OBJ_VAL(copy_string(name->start, name->length)));
}

bool identifiers_equal(Token* a, Token* b) {
    return (a->length == b->length) && (std::memcmp(a->start, b->start, a->length) == 0);
}


int resolve_local(Compiler* compiler, Token* name) {
    for (int i = compiler->local_count - 1; i >= 0; i--) {
        Local* local = &compiler->locals[i];
        if (identifiers_equal(name, &local->name)) return i;
        if (local->depth == -1) {
            parser.error("Cannot read local variable in its own initializer.");
        }
    }
    return -1;
}

void add_local(Token name) {
    if (current->local_count == UINT8_COUNT) {
        parser.error("Too many local variables in function.");
        return;
    }
    Local* local = &current->locals[current->local_count++];
    local->name = name;
    local->depth = -1;
}

void declare_variable() {
    // Global variables are implicitly declared
    if (current->scope_depth == 0) return;

    Token* name = &parser.previous;

    for (int i = current->local_count - 1; i >= 0; i--) {
        Local* local = &current->locals[i];
        if (local->depth != -1 && local->depth < current->scope_depth) break;
        if (identifiers_equal(name, &local->name)) {
            parser.error("Already a variable with this name in this scope.");
        }
    }

    add_local(*name);
}

uint8_t parser_variable(const char* error_msg) {
    parser.consume(IDENTIFIER, error_msg);

    declare_variable();
    if (current->scope_depth > 0) return 0;

    return identifier_constant(&parser.previous);
}

void mark_initialized() {
    if (current->scope_depth == 0) return;
    current->locals[current->local_count - 1].depth = current->scope_depth;
}

void define_variable(uint8_t global) {
    if (current->scope_depth > 0) {
        mark_initialized();
        return;
    }
    emit_bytes(OP_DEFINE_GLOBAL, global);
}

void and_(bool can_assign) {
    int end_jump = emit_jump(OP_JUMP_IF_FALSE);

    emit_byte(OP_POP);
    parse_precedence(PREC_AND);

    patch_jump(end_jump);
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

void block() {
    while (!parser.check(RIGHT_BRACE) && !parser.check(EOF_TOKEN)) {
        declaration();
    }
    parser.consume(RIGHT_BRACE, "Expect '}' after block.");
}

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

void if_statement() {
    parser.consume(LEFT_PAREN, "Expect '(' after 'if'.");
    expression();
    parser.consume(RIGHT_PAREN, "Expect ')' after condition.");

    int then_jump = emit_jump(OP_JUMP_IF_FALSE); 
    emit_byte(OP_POP);
    statement();

    int else_jump = emit_jump(OP_JUMP);

    patch_jump(then_jump);
    emit_byte(OP_POP);

    if (parser.match(ELSE)) statement();
    patch_jump(else_jump);
}

void info_statement() {
    expression();
    parser.consume(SEMICOLON, "Expect ';' after value.");
    emit_byte(OP_INFO);
}

void using_statement() {
    parser.consume(STRING, "Expect string after 'using'.");
    ObjString* moduleName = copy_string(parser.previous.start + 1, parser.previous.length - 2);
    parser.consume(SEMICOLON, "Expect ';' after value.");
    emit_constant(OBJ_VAL(moduleName));
    emit_byte(OP_IMPORT);
}

void synchronize() {
    parser.panic_mode = false;

    while (parser.current.kind != EOF_TOKEN) {
        if (parser.previous.kind == SEMICOLON) return;
        if (return_context.find(parser.current.kind) != return_context.end()) return;
        parser.advance();
    }
}

void declaration() {
    if (parser.panic_mode) synchronize();
    else if (parser.match(HAVE)) { var_declaration(); }
    else { statement(); }
}

void statement() {
    if (parser.match(INFO))    info_statement();
    else if (parser.match(IF)) if_statement();
    else if (parser.match(USING)) using_statement();
    else if (parser.match(LEFT_BRACE)) { begin_scope(); block(); end_scope(); }
    else { expression_statement(); }
}

void grouping(bool can_assign) {
    expression();
    parser.consume(RIGHT_PAREN, "Expect ')' after expression.");
}

void named_variable(Token name, bool can_assign) {
    uint8_t get_op, set_op;
    int arg = resolve_local(current, &name);
    if (arg != -1) {
        get_op = OP_GET_LOCAL;
        set_op = OP_SET_LOCAL;
    } else {
        arg = identifier_constant(&name);
        get_op = OP_GET_GLOBAL;
        set_op = OP_SET_GLOBAL;
    }
    
    if (parser.match(EQUAL) && can_assign) {
        expression();
        emit_bytes(set_op, (uint8_t)arg);
    } else {
        emit_bytes(get_op, (uint8_t)arg);
    }
}

void _number(bool can_assign) {
    double value = std::strtod(parser.previous.start, nullptr);
    emit_constant(NUMBER_VAL(value));
}

void or_(bool can_assign) {
    int else_jump = emit_jump(OP_JUMP_IF_FALSE);
    int end_jump = emit_jump(OP_JUMP);

    patch_jump(else_jump);
    emit_byte(OP_POP);

    parse_precedence(PREC_OR);
    patch_jump(end_jump);
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
    Compiler compiler;
    init_compiler(&compiler); 
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