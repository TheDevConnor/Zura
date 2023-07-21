#include <cstdint>
#include <iostream>
#include <string>

#include "../garbage_collector/gc.h"
#include "declarations/declaration.h"
#include "declarations/expr/expr.h"
#include "helper/parser_helper.h"
#include "lexer/tokens.h"
#include "parser.h"

void init_compiler(Compiler *compiler, FunctionType type) {
  compiler->enclosing = current;
  compiler->function = nullptr;
  compiler->type = type;
  compiler->local_count = 0;
  compiler->scope_depth = 0;
  compiler->function = new_function();
  current = compiler;

  if (type != TYPE_SCRIPT)
    current->function->name =
        copy_string(parser.previous.start, parser.previous.length);

  Local *local = &current->locals[current->local_count++];
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

ObjFunction *end_compiler() {
  emit_return();
  ObjFunction *function = current->function;
#ifndef DEBUG_PRINT_CODE
#include "../debug/debug.h"
  if (!parser.had_error)
    disassemble_chunk(compiling_chunk(), function->name != nullptr
                                             ? function->name->chars
                                             : "<script>");
#endif

  current = current->enclosing;
  return function;
}

void end_scope() {
  current->scope_depth--;

  while (current->local_count > 0 &&
         current->locals[current->local_count - 1].depth >
             current->scope_depth) {
    emit_byte(OP_POP);
    if (current->locals[current->local_count - 1].is_captured)
      emit_byte(OP_CLOSE_UPVALUE);
    else
      emit_byte(OP_POP);
    current->local_count--;
  }
}

uint8_t identifier_constant(Token *name) {
  return make_constant(OBJ_VAL(copy_string(name->start, name->length)));
}

bool identifiers_equal(Token *a, Token *b) {
  return (a->length == b->length) &&
         (memcmp(a->start, b->start, a->length) == 0);
}

int resolve_local(Compiler *compiler, Token *name) {
  for (int i = compiler->local_count - 1; i >= 0; i--) {
    Local *local = &compiler->locals[i];
    if (identifiers_equal(name, &local->name))
      return i;
    if (local->depth == -1) {
      parser.error("Cannot read local variable in its own initializer.");
    }
  }
  return -1;
}

int add_upvalue(Compiler *compiler, uint8_t index, bool is_local) {
  int up_value_count = compiler->function->upvalue_count;

  for (int i = 0; i < up_value_count; i++) {
    Upvalue *up_value = &compiler->upvalues[i];
    if (up_value->index == index && up_value->is_local == is_local)
      return i;
  }

  if (up_value_count == UINT8_COUNT) {
    parser.error("Too many closure variables in function.");
    return 0;
  }

  compiler->upvalues[up_value_count].is_local = is_local;
  compiler->upvalues[up_value_count].index = index;
  return compiler->function->upvalue_count++;
}

int resolve_upvalue(Compiler *compiler, Token *name) {
  if (compiler->enclosing == nullptr)
    return -1;

  int local = resolve_local(compiler->enclosing, name);
  if (local != -1) {
    compiler->enclosing->locals[local].is_captured = true;
    return add_upvalue(compiler, static_cast<uint8_t>(local), true);
  }

  int upvalue = resolve_upvalue(compiler->enclosing, name);
  if (upvalue != -1)
    return add_upvalue(compiler, static_cast<uint8_t>(upvalue), false);

  return -1;
}

void add_local(Token name) {
  if (current->local_count == UINT8_COUNT) {
    parser.error("Too many local variables in function.");
    return;
  }
  Local *local = &current->locals[current->local_count++];
  local->name = name;
  local->depth = -1;
  local->is_captured = false;
}

void declare_variable() {
  // Global variables are implicitly declared
  if (current->scope_depth == 0)
    return;

  Token *name = &parser.previous;

  for (int i = current->local_count - 1; i >= 0; i--) {
    Local *local = &current->locals[i];
    if (local->depth != -1 && local->depth < current->scope_depth)
      break;
    if (identifiers_equal(name, &local->name)) {
      parser.error("Already a variable with this name in this scope.");
    }
  }

  add_local(*name);
}

uint8_t parser_variable(const char *error_msg) {
  parser.consume(IDENTIFIER, error_msg);

  declare_variable();
  if (current->scope_depth > 0)
    return 0;

  return identifier_constant(&parser.previous);
}

void mark_initialized() {
  if (current->scope_depth == 0)
    return;
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
  if (!parser.check(RIGHT_PAREN)) {
    do {
      expression();
      if (arg_count == 255)
        parser.error("Can't have more than 255 arguments");
      arg_count++;
    } while (parser.match(COMMA));
  }
  parser.consume(RIGHT_PAREN, "Expected a ')' after arguments");
  return arg_count;
}

void binary(bool can_assign) {
  (void)can_assign;
  // Remember the operator
  TokenKind operator_type = parser.previous.kind;

  // Compile the right operand
  ParseRule *rule = get_rule(operator_type);
  parse_precedence(static_cast<Precedence>(rule->precedence + 1));

  // Emit the operator instruction
  switch (operator_type) {
  case PLUS:
    emit_byte(OP_ADD);
    break;
  case MINUS:
    emit_byte(OP_SUBTRACT);
    break;
  case STAR:
    emit_byte(OP_MULTIPLY);
    break;
  case SLASH:
    emit_byte(OP_DIVIDE);
    break;
  case MODULO:
    emit_byte(OP_MODULO);
    break;
  case POWER:
    emit_byte(OP_POWER);
    break;

  // Comparison operators
  case BANG_EQUAL:
    emit_bytes(OP_EQUAL, OP_NOT);
    break; // !=
  case EQUAL_EQUAL:
    emit_byte(OP_EQUAL);
    break; // ==
  case GREATER:
    emit_byte(OP_GREATER);
    break; // >
  case GREATER_EQUAL:
    emit_bytes(OP_LESS, OP_NOT);
    break; // >=
  case LESS:
    emit_byte(OP_LESS);
    break; // <
  case LESS_EQUAL:
    emit_bytes(OP_GREATER, OP_NOT);
    break; // <=

  default:
    return; // Unreachable
  }
}

void literal(bool can_assign) {
  (void)can_assign;
  switch (parser.previous.kind) {
  case FALSE:
    emit_byte(OP_FALSE);
    break;
  case TRUE:
    emit_byte(OP_TRUE);
    break;
  case NIL:
    emit_byte(OP_NIL);
    break;
  default:
    return; // Unreachable
  }
}

void declaration() {
  if (parser.panic_mode)
    synchronize();
  else if (parser.match(CLASS)) {
    class_declaration();
  } else if (parser.match(FUNC)) {
    func_declaration();
  } else if (parser.match(HAVE)) {
    var_declaration();
  } else {
    statement();
  }
}

void statement() {
  // Control statements
  if (parser.match(INFO))
    info_statement();
  else if (parser.match(RETURN))
    return_statement();
  // Conditional statements
  else if (parser.match(IF))
    if_statement();
  // Loop statements
  else if (parser.match(CONTINUE))
    continue_statement();
  else if (parser.match(BREAK))
    break_statement();
  else if (parser.match(WHILE))
    while_statement();
  else if (parser.match(FOR))
    for_statement();
  // Import statements
  else if (parser.match(USING))
    using_statement();
  // Block statements
  else if (parser.match(LEFT_BRACE)) {
    begin_scope();
    block();
    end_scope();
  }
  // Other Statements
  else {
    expression_statement();
  }
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

ParseRule *get_rule(TokenKind kind) { return &rules[kind]; }

ObjFunction *compile(const char *source) {
  init_tokenizer(source);
  Compiler compiler;
  init_compiler(&compiler, TYPE_SCRIPT);

  parser.had_error = false;
  parser.panic_mode = false;

  parser.advance();

  while (!parser.match(EOF_TOKEN)) {
    declaration();
  }

  ObjFunction *function = end_compiler();
  return parser.had_error ? nullptr : function;
}

void mark_compiler_roots() {
  Compiler *compiler = current;
  while (compiler != nullptr) {
    mark_object((Obj *)compiler->function);
    compiler = compiler->enclosing;
  }
}
