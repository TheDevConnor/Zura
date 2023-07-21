#include "../../helper/parser_helper.h"
#include "../../chunk.h"

void expression_statement() {
  expression();
  parser.consume(SEMICOLON, "Expect ';' after expression.");
  emit_byte(OP_POP);
}

void block() {
  while (!parser.check(RIGHT_BRACE) && !parser.check(EOF_TOKEN)) {
    declaration();
  }
  parser.consume(RIGHT_BRACE, "Expect '}' after block.");
}


void info_statement() {
  expression();
  parser.consume(SEMICOLON, "Expect ';' after value.");
  emit_byte(OP_INFO);
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
  } else if (parser.match(SEMICOLON))
    ; // No initializer
  else
    expression_statement();

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
  if (!parser.match(RIGHT_PAREN)) {
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
  if (loop_variable != -1) {
    begin_scope();
    emit_bytes(OP_GET_LOCAL, (uint8_t)loop_variable);
    add_local(loop_variable_name);
    mark_initialized();
    inner_varuable = current->local_count - 1;
  }

  statement();

  if (loop_variable != -1) {
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

  if (parser.match(ELSE))
    statement();
  patch_jump(else_jump);
}

void input_statement(bool can_assign) {
  (void)can_assign;
  expression();
  emit_byte(OP_INPUT);
}

void return_statement() {
  if (current->type == TYPE_SCRIPT)
    parser.error("Can't return from top-level code!");
  if (parser.match(SEMICOLON))
    emit_return();
  else {
    if (current->type == TYPE_INITIALIZER)
      parser.error("Can't return a value from an initializer.");
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
  for (int i = current->local_count - 1;
       i >= 0 && current->locals[i].depth > inner_most_loop_scope_depth; i--) {
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
  for (int i = current->local_count - 1;
       i >= 0 && current->locals[i].depth > inner_most_loop_scope_depth; i--) {
    emit_byte(OP_POP);
  }
  emit_byte(OP_BREAK);
  parser.consume(SEMICOLON, "Expect ';' after 'break'.");
}

void using_statement() {
  parser.consume(STRING, "Expect string after 'using'.");
  ObjString *moduleName =
      copy_string(parser.previous.start + 1, parser.previous.length - 2);
  parser.consume(SEMICOLON, "Expect ';' after value.");
  emit_constant(OBJ_VAL(moduleName));
  emit_byte(OP_IMPORT);
}