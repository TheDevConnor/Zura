#pragma once

#include <string>
#include <unordered_map>

#include "../helper/import.h"
#include "helper.h"
#include "tokens.h"

Token scan_token() {
  skip_whitespace();
  scanner.start = scanner.current;

  if (is_at_end())
    return make_token(EOF_TOKEN);

  char c = advance();

  if (isdigit(c))
    return number();
  if (isalpha(c))
    return identifier();

  switch (c) {
  case ')':
    return make_token(RIGHT_PAREN);
  case '(':
    return make_token(LEFT_PAREN);
  case '{':
    return make_token(LEFT_BRACE);
  case '}':
    return make_token(RIGHT_BRACE);
  case '[':
    return make_token(LEFT_BRACKET);
  case ']':
    return make_token(RIGHT_BRACKET);
  case ';':
    return make_token(SEMICOLON);
  case ',':
    return make_token(COMMA);
  case '.':
    return make_token(DOT);
  case '/':
    return make_token(SLASH);
  case '%':
    return make_token(MODULO);
  case '#':
    return make_token(HASHTAG);
  case '~':
    return make_token(TILDE);
  case '+':
    return make_token(match('+') ? INCREMENT : PLUS);
  case '-':
    return make_token(match('-') ? DECREMENT : MINUS);
  case '&':
    return make_token(match('&') ? AND : BIT_AND);
  case '|':
    return make_token(match('|') ? OR : BIT_OR);
  case '*':
    return make_token(match('*') ? POWER : STAR);
  case '!':
    return make_token(match('=') ? BANG_EQUAL : BANG);
  case '=':
    return make_token(match('=') ? EQUAL_EQUAL : EQUAL);
  case ':':
    return make_token(match('=') ? WALRUS : COLON);
  case '>':
    return make_token(match('=') ? GREATER_EQUAL : GREATER);
  case '<':
    return make_token(match('=') ? LESS_EQUAL : LESS);
  case '"':
    return _string();
  }

  error_token("Unexpected character.");
  return make_token(ERROR_TOKEN);
}
