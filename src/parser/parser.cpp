#include <iostream>
#include <string>

#include "../garbage_collector/gc.h"
#include "helper/parser_helper.h"
#include "parser.h"

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
    if (offset > UINT16_MAX) parser.error("Loop body too large.");

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
    if(current->type == TYPE_INITIALIZER) emit_bytes(OP_GET_LOCAL, 0);
    else emit_byte(OP_NIL);
    emit_byte(OP_RETURN);
}

void emit_constant(Value v) {
    emit_bytes(OP_CONSTANT, make_constant(v));
}

void patch_jump(int offset) {
    // -2 to adjust for the bytecode for the jump offset itself.
    int jump = compiling_chunk()->count - offset - 2;

    if (jump > UINT16_MAX) {
        parser.error("Too much code to jump over.");
    }

    compiling_chunk()->code[offset] = (jump >> 8) & 0xff;
    compiling_chunk()->code[offset + 1] = jump & 0xff;
}

void init_compiler(Compiler* compiler, FunctionType type) {
    compiler->enclosing = current;
    compiler->function = nullptr;
    compiler->type = type;
    compiler->local_count = 0;
    compiler->scope_depth = 0;
    compiler->function = new_function();
    current = compiler;

    if(type != TYPE_SCRIPT) 
        current->function->name = copy_string(parser.previous.start, parser.previous.length);

    Local* local = &current->locals[current->local_count++];
    local->depth = 0;
    local->is_captured = false;
    if (type != TYPE_FUNCTION) {
        local->name.start = "this";
        local->name.length = 4;
    } else {
        local->name.start = "";
        local->name.length = 0;
    }
}

ObjFunction* end_compiler() {
    emit_return();
    ObjFunction* function = current->function;
    #ifndef DEBUG_PRINT_CODE
        #include "../debug/debug.h"
        if (!parser.had_error) disassemble_chunk(compiling_chunk(), 
            function->name != nullptr ? function->name->chars : "<script>");
    #endif 

    current = current->enclosing;
    return function;
}

void begin_scope() { current->scope_depth++; }
void end_scope()   { 
    current->scope_depth--; 

    while(current->local_count > 0 && current->locals[current->local_count - 1].depth > current->scope_depth) {
        emit_byte(OP_POP);
        if(current->locals[current->local_count - 1].is_captured) emit_byte(OP_CLOSE_UPVALUE);
        else emit_byte(OP_POP);
        current->local_count--;
    }
}

void expression();
void statement();
void declaration();
ParseRule* get_rule(TokenKind kind);
void parse_precedence(Precedence precedence);
void named_variable(Token name, bool can_assign);

uint8_t identifier_constant(Token* name) {
    return make_constant(OBJ_VAL(copy_string(name->start, name->length)));
}

bool identifiers_equal(Token* a, Token* b) {
    return (a->length == b->length) && (memcmp(a->start, b->start, a->length) == 0);
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

int add_upvalue(Compiler* compiler, uint8_t index, bool is_local) {
    int up_value_count = compiler->function->upvalue_count;

    for (int i = 0; i < up_value_count; i++) {
        Upvalue* up_value = &compiler->upvalues[i];
        if (up_value->index == index && up_value->is_local == is_local) return i;
    }

    if (up_value_count == UINT8_COUNT) {
        parser.error("Too many closure variables in function."); 
        return 0;
    }

    compiler->upvalues[up_value_count].is_local = is_local;
    compiler->upvalues[up_value_count].index = index;
    return compiler->function->upvalue_count++;
}

int resolve_upvalue(Compiler* compiler, Token* name) {
    if (compiler->enclosing == nullptr) return -1;

    int local = resolve_local(compiler->enclosing, name);
    if (local != -1) {
        compiler->enclosing->locals[local].is_captured = true;
        return add_upvalue(compiler, static_cast<uint8_t>(local), true);
    }

    int upvalue = resolve_upvalue(compiler->enclosing, name);
    if (upvalue != -1) return add_upvalue(compiler, static_cast<uint8_t>(upvalue), false);

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
    local->is_captured = false;
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

uint8_t argument_list() {
    uint8_t arg_count = 0;
    if(!parser.check(RIGHT_PAREN)) {
        do {
            expression();
            if(arg_count == 255) parser.error("Can't have more than 255 arguments");
            arg_count++;
        } while(parser.match(COMMA));
    }
    parser.consume(RIGHT_PAREN, "Expected a ')' after arguments");
    return arg_count;
}

void and_(bool can_assign) {
    (void)can_assign;
    int end_jump = emit_jump(OP_JUMP_IF_FALSE);

    emit_byte(OP_POP);
    parse_precedence(PREC_AND);

    patch_jump(end_jump);
}

void binary(bool can_assign) {
    (void)can_assign;
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
    (void)can_assign;
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

void function(FunctionType type) {
    Compiler compiler;
    init_compiler(&compiler, type);
    begin_scope();

    parser.consume(LEFT_PAREN, "Expected a '(' after a function name!");
    if (!parser.check(RIGHT_PAREN)) {
        do {
            current->function->arity++;
            if(current->function->arity > 255) parser.error_at_current("Can't have more than 255 parameters!");
            uint8_t constant = parser_variable("Expected paramerter name!");
            define_variable(constant);
        } while (parser.match(COMMA));
    }
    parser.consume(RIGHT_PAREN, "Expected a ')' after parameters!");
    parser.consume(LEFT_BRACE, "Expected a '{' after a function body!");
    block();

    ObjFunction* function = end_compiler();
    emit_bytes(OP_CLOSURE, make_constant(OBJ_VAL(function)));
}

void method() {
    parser.consume(IDENTIFIER, "Expected a method name!");
    uint8_t constant = identifier_constant(&parser.previous);

    FunctionType type = TYPE_METHOD;
    if(parser.previous.length == 4 && memcmp(parser.previous.start, "init", 4) == 0) 
        type = TYPE_INITIALIZER;
    function(type);

    emit_bytes(OP_METHOD, constant);
}

void class_declaration() {
    parser.consume(IDENTIFIER, "Expect class name!");
    Token class_name = parser.previous;
    uint8_t name_constant = identifier_constant(&parser.previous);
    declare_variable();

    emit_bytes(OP_CLASS, name_constant);
    define_variable(name_constant);

    ClassCompiler class_compiler;
    class_compiler.enclosing = current_class;
    current_class = &class_compiler;

    named_variable(class_name, false);
    parser.consume(LEFT_BRACE, "Expected '{' before class body");
    while (!parser.check(RIGHT_BRACE) && !parser.check(EOF_TOKEN)) {
        method();
    }
    parser.consume(RIGHT_BRACE, "Expected '}' after class body");
    emit_byte(OP_POP);

    current_class = current_class->enclosing;
}

void func_declaration() {
    uint8_t global = parser_variable("Expected a function name!");
    mark_initialized();
    function(TYPE_FUNCTION);
    define_variable(global);
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

void for_statement() {
    begin_scope();

    int loop_variable = -1;
    Token loop_variable_name;
    loop_variable_name.start = nullptr;

    parser.consume(LEFT_PAREN, "Expect '(' after 'for'.");
    if (parser.match(HAVE)) {
        loop_variable_name = parser.current;
        var_declaration();
        loop_variable = current->local_count - 1;
    }
    else if (parser.match(SEMICOLON)); // No initializer
    else expression_statement();

    int surrounding_loop_start = inner_most_loop_start;
    int surrounding_loop_scope = inner_most_loop_scope_depth;
    inner_most_loop_start = compiling_chunk()->count;
    inner_most_loop_scope_depth = current->scope_depth;

    int exit_jump = -1;
    if (!parser.match(SEMICOLON)) {
        expression();
        parser.consume(SEMICOLON, "Expect ';' after loop condition.");

        // Jump out of the loop if the condition is false
        exit_jump = emit_jump(OP_JUMP_IF_FALSE);
        emit_byte(OP_POP); // Condition
    }

    int body_jump = -1;
    if(!parser.match(RIGHT_PAREN)) {
        body_jump = emit_jump(OP_JUMP);

        int increment_start = compiling_chunk()->count;
        expression();
        emit_byte(OP_POP);
        parser.consume(RIGHT_PAREN, "Expect ')' after for clauses.");

        emit_loop(inner_most_loop_start);
        inner_most_loop_start = increment_start;
        patch_jump(body_jump);
    }

    int inner_varuable = -1;
    if(loop_variable != -1) {
        begin_scope();
        emit_bytes(OP_GET_LOCAL, (uint8_t)loop_variable);
        add_local(loop_variable_name);
        mark_initialized();
        inner_varuable = current->local_count - 1;
    }

    statement();

    if(loop_variable != -1) {
        emit_bytes(OP_GET_LOCAL, (uint8_t)inner_varuable);
        emit_bytes(OP_SET_LOCAL, (uint8_t)loop_variable);
        emit_byte(OP_POP);
    }

    emit_loop(inner_most_loop_start);

    // Patch the exit jump
    if (exit_jump != -1) {
        patch_jump(exit_jump);
        emit_byte(OP_POP); // Condition
    }

    inner_most_loop_start = surrounding_loop_start;
    inner_most_loop_scope_depth = surrounding_loop_scope;

    end_scope();
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

void return_statement() {
    if (current->type == TYPE_SCRIPT) parser.error("Can't return from top-level code!");
    if (parser.match(SEMICOLON)) emit_return();
    else {
        if(current->type == TYPE_INITIALIZER) parser.error("Can't return a value from an initializer.");
        expression();
        parser.consume(SEMICOLON, "Expected ';' after the return value");
        emit_byte(OP_RETURN);
    }
}

void while_statement() {
    int loop_start = compiling_chunk()->count;
    parser.consume(LEFT_PAREN, "Expect '(' after 'while'.");
    expression();
    parser.consume(RIGHT_PAREN, "Expect ')' after condition.");

    int exit_jump = emit_jump(OP_JUMP_IF_FALSE);
    emit_byte(OP_POP);
    statement();
    emit_loop(loop_start);

    patch_jump(exit_jump);
    emit_byte(OP_POP);
}

void continue_statement() {
    if (inner_most_loop_start == -1) {
        parser.error("Cannot use 'continue' outside of a loop.");
        return;
    }
    // Discard any local variables created in the loop
    for (int i = current->local_count - 1; i >= 0 && current->locals[i].depth > inner_most_loop_scope_depth; i--) {
        emit_byte(OP_POP);
    }
    emit_loop(inner_most_loop_start);
    parser.consume(SEMICOLON, "Expect ';' after 'continue'.");
}

void break_statement() {
    if (inner_most_loop_start == -1) {
        parser.error("Cannot use 'break' outside of a loop.");
        return;
    }
    // Discard any local variables created in the loop
    for (int i = current->local_count - 1; i >= 0 && current->locals[i].depth > inner_most_loop_scope_depth; i--) {
        emit_byte(OP_POP);
    }
    emit_byte(OP_BREAK);
    parser.consume(SEMICOLON, "Expect ';' after 'break'.");
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
    else if (parser.match(CLASS)) { class_declaration();}
    else if (parser.match(FUNC))  { func_declaration(); }
    else if (parser.match(HAVE))  { var_declaration();  }
    else { statement(); }
}

void statement() {
    // Control statements
    if (parser.match(INFO))    info_statement();
    else if (parser.match(RETURN)) return_statement();
    // Conditional statements
    else if (parser.match(IF)) if_statement();
    // Loop statements
    else if (parser.match(CONTINUE)) continue_statement();
    else if (parser.match(BREAK)) break_statement();
    else if (parser.match(WHILE)) while_statement();
    else if (parser.match(FOR)) for_statement();
    // Import statements
    else if (parser.match(USING)) using_statement();
    // Block statements
    else if (parser.match(LEFT_BRACE)) { begin_scope(); block(); end_scope(); }
    // Other Statements
    else { expression_statement(); }
}

void grouping(bool can_assign) {
    (void)can_assign;
    expression();
    parser.consume(RIGHT_PAREN, "Expect ')' after expression.");
}

void named_variable(Token name, bool can_assign) {
    uint8_t get_op, set_op;
    int arg = resolve_local(current, &name);
    if (arg != -1) {
        get_op = OP_GET_LOCAL;
        set_op = OP_SET_LOCAL;
    } else if ((arg = resolve_upvalue(current, &name)) != -1) {
        get_op = OP_GET_UPVALUE;
        set_op = OP_SET_UPVALUE;
    } else if ((arg = identifier_constant(&name)) != -1) {
        get_op = OP_GET_GLOBAL;
        set_op = OP_SET_GLOBAL;
    } else {
        string message = "Undefined variable name '";
        message += name.start;
        message += "'.";
        parser.error(message.c_str());
        return;
    }
    
    if (parser.match(EQUAL) && can_assign) {
        expression();
        emit_bytes(set_op, (uint8_t)arg);
    } else {
        emit_bytes(get_op, (uint8_t)arg);
    }
}

void _number(bool can_assign) {
    (void)can_assign;
    double value = strtod(parser.previous.start, nullptr);
    emit_constant(NUMBER_VAL(value));
}

void or_(bool can_assign) {
    (void)can_assign;
    int else_jump = emit_jump(OP_JUMP_IF_FALSE);
    int end_jump = emit_jump(OP_JUMP);

    patch_jump(else_jump);
    emit_byte(OP_POP);

    parse_precedence(PREC_OR);
    patch_jump(end_jump);
}

void _string(bool can_assign) {
    (void)can_assign;
    emit_constant(OBJ_VAL(copy_string(parser.previous.start + 1, parser.previous.length - 2)));
}

void _variable(bool can_assign) {
    (void)can_assign;
    named_variable(parser.previous, can_assign);
}

void _this(bool can_assign) {
    (void)can_assign;
    if(current_class == nullptr) {
        parser.error("Cannot use 'this' outside of a class.");
        return;
    }
    _variable(false);
}

void unary(bool can_assign) {
    (void)can_assign;
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

void call(bool can_assign) {
    (void)can_assign;
    uint8_t arg_count = argument_list();
    emit_bytes(OP_CALL, arg_count);
}

void dot(bool can_assign) {
    parser.consume(IDENTIFIER, "Exactly property name after '.'");
    uint8_t name = identifier_constant(&parser.previous);

    if(can_assign && parser.match(EQUAL)) {
        expression();
        emit_bytes(OP_SET_PROPERTY, name);
    }
    else emit_bytes(OP_GET_PROPERTY, name);
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

ObjFunction* compile(const char* source) {
    init_tokenizer(source);
    Compiler compiler;
    init_compiler(&compiler, TYPE_SCRIPT);

    parser.had_error = false;
    parser.panic_mode = false;

    parser.advance();

    while (!parser.match(EOF_TOKEN)) {
        declaration();
    }

    ObjFunction* function = end_compiler();
    return parser.had_error ? nullptr : function;
}

void mark_compiler_roots() {
    Compiler* compiler = current;
    while(compiler != nullptr) {
        mark_object((Obj*)compiler->function);
        compiler = compiler->enclosing;
    }
}