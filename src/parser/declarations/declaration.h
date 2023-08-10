#pragma once

#include "../helper/parser_helper.h"
#include "stmt/stmt.h"

void function(FunctionType type) {
  Compiler compiler;
  init_compiler(&compiler, type);
  begin_scope();

  parser.consume(LEFT_PAREN, "Expected a '(' after a function name!");
  if (!parser.check(RIGHT_PAREN)) {
    do {
      current->function->arity++;
      if (current->function->arity > 255)
        parser.error_at_current("Can't have more than 255 parameters!");
      uint8_t constant = parser_variable("Expected paramerter name!");
      define_variable(constant);
    } while (parser.match(COMMA));
  }
  parser.consume(RIGHT_PAREN, "Expected a ')' after parameters!");
  parser.consume(LEFT_BRACE, "Expected a '{' after a function body!");
  block();

  ObjFunction *function = end_compiler();
  emit_bytes(OP_CLOSURE, make_constant(OBJ_VAL(function)));
}

void method() {
  parser.consume(IDENTIFIER, "Expected a method name!");
  uint8_t constant = identifier_constant(&parser.previous);

  FunctionType type = TYPE_METHOD;
  if (parser.previous.length == 4 &&
      memcmp(parser.previous.start, "init", 4) == 0)
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
  class_compiler.has_superclass = false;
  class_compiler.enclosing = current_class;
  current_class = &class_compiler;

  if (parser.match(INHERITANCE)) {
    parser.consume(IDENTIFIER, "Expect superclass name!");
    _variable(false);

    if (identifiers_equal(&class_name, &parser.previous))
      parser.error("A class can't inherit from itself!");

    begin_scope();
    add_local(synthetic_token("super"));
    define_variable(0);

    named_variable(class_name, false);
    emit_byte(OP_INHERIT);
    class_compiler.has_superclass = true;
  }

  named_variable(class_name, false);
  parser.consume(LEFT_BRACE, "Expected '{' before class body");
  while (!parser.check(RIGHT_BRACE) && !parser.check(EOF_TOKEN)) {
    method();
  }
  parser.consume(RIGHT_BRACE, "Expected '}' after class body");
  emit_byte(OP_POP);

  if (class_compiler.has_superclass)
    end_scope();

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

  if (parser.match(EQUAL))
    parser.error("You must use a ':=' to declare a variable! While '=' is used "
                 "for reassignment.");
  if (parser.match(COLON))
    parser.error("You must use a ':=' to declare a variable! While ':' is used "
                 "for type annotation.");

  if (parser.match(WALRUS))
    expression();
  else
    emit_byte(OP_NIL);

  parser.consume(SEMICOLON, "Expect ';' after variable declaration.");
  define_variable(global);
}

void static_var_decleration() {
  uint8_t global = parser_variable("Expect variable name.");

  if (parser.match(EQUAL))
    parser.error("You must use a ':=' to declare a variable! While '=' is used "
                 "for reassignment.");
  if (parser.match(COLON))
    parser.error("You must use a ':=' to declare a variable! While ':' is used "
                 "for type annotation.");

  if (parser.match(WALRUS))
    expression();
  else
    emit_byte(OP_NIL);

  parser.consume(SEMICOLON, "Expect ';' after variable declaration.");
  define_static_variable(global);
}

void synchronize() {
  parser.panic_mode = false;

  while (parser.current.kind != EOF_TOKEN) {
    if (parser.previous.kind == SEMICOLON)
      return;
    if (return_context.find(parser.current.kind) != return_context.end())
      return;
    parser.advance();
  }
}
