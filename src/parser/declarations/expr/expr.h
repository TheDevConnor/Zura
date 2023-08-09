#pragma once

#include "../../helper/parser_helper.h"
#include "../../chunk.h"

void _number(bool can_assign) {
  (void)can_assign;
  double value = strtod(parser.previous.start, nullptr);
  emit_constant(NUMBER_VAL(value));
}

void and_(bool can_assign) {
  (void)can_assign;
  int end_jump = emit_jump(OP_JUMP_IF_FALSE);

  emit_byte(OP_POP);
  parse_precedence(PREC_AND);

  patch_jump(end_jump);
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
  emit_constant(OBJ_VAL(
      copy_string(parser.previous.start + 1, parser.previous.length - 2)));
}

void _variable(bool can_assign) {
  (void)can_assign;
  named_variable(parser.previous, can_assign);
}

Token synthetic_token(const char *text) {
  Token token;
  token.start = text;
  token.length = (int)strlen(text);
  return token;
}

void super_(bool can_assign) {
  (void)can_assign;
  if(current_class == nullptr) parser.error("Cannot use 'super' outside of a class.");
  else if(!current_class->has_superclass) parser.error("Cannot use 'super' in a class with no superclass.");

  parser.consume(DOT, "Expect '.' after 'super'.");
  parser.consume(IDENTIFIER, "Expect superclass method name.");
  uint8_t name = identifier_constant(&parser.previous);

  named_variable(synthetic_token("this"), false);
  if(parser.match(LEFT_PAREN)) {
    uint8_t arg_count = argument_list();
    named_variable(synthetic_token("super"), false);
    emit_bytes(OP_SUPER_INVOKE, name);
    emit_byte(arg_count);
  } else {
    named_variable(synthetic_token("super"), false);
    emit_bytes(OP_GET_SUPER, name);
  }
}

void _this(bool can_assign) {
  (void)can_assign;
  if (current_class == nullptr) {
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
  case BANG:
    emit_byte(OP_NOT);
    break;
  case MINUS:
    emit_byte(OP_NEGATE);
    break;
  case INCREMENT:
    emit_byte(OP_INCREMENT);
    break;
  case DECREMENT:
    emit_byte(OP_DECREMENT);
    break;
  default:
    return; // Unreachable
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

  if (can_assign && parser.match(EQUAL)) {
    expression();
    emit_bytes(OP_SET_PROPERTY, name);
  } else if (parser.match(LEFT_PAREN)) {
    uint8_t arg_count = argument_list();
    emit_bytes(OP_INVOKE, name);
    emit_byte(arg_count);
  } else
    emit_bytes(OP_GET_PROPERTY, name);
}

void array_literal(bool can_assign) {
  (void)can_assign;
  int num_elements = 0;

  while (!parser.check(RIGHT_BRACKET) && !parser.check(EOF_TOKEN)) {
    expression();
    if (num_elements > 255) {
      parser.error("Cannot have more than 255 elements in an array.");
    }
    num_elements++;
    if (!parser.match(COMMA)) {
      break;
    }
  }
  parser.consume(RIGHT_BRACKET, "Expect ']' after array elements.");
  emit_byte(OP_ARRAY);
  emit_byte((uint8_t)num_elements);
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

  if (can_assign && match(TK_INPUT)) {
    input_statement(true);
  }

  // Check for the array literal
  if (can_assign && parser.match(LEFT_BRACKET)) {
    array_literal(can_assign);
  }
}
