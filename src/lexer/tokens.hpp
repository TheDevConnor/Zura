#pragma once

#include <string>

#include "../types/type.hpp"

namespace Element {

enum TokenKind {
  // Single-character Tokens!
  LEFT_PAREN,    // (
  RIGHT_PAREN,   // )
  LEFT_BRACE,    // {
  RIGHT_BRACE,   // }
  LEFT_BRACKET,  // [
  RIGHT_BRACKET, // ]
  COMMA,         // ,
  DOT,           // .
  MINUS,         // -
  PLUS,          // +
  SEMICOLON,     // ;
  SLASH,         // /
  STAR,          // *
  HASHTAG,       // #
  COLON,         // :
  MODULO,        // %
  DOLLAR,        // $
  POWER,         // **
  TILDE,         // ~
  AT,            // @

  // One or two character tokens!
  BANG,          // !
  BANG_EQUAL,    // !=
  EQUAL,         // =
  GREATER,       // >
  GREATER_EQUAL, // >=
  LESS,          // <
  LESS_EQUAL,    // <=
  WALRUS,        // :=
  EXTENDS,       // extends
  INCREMENT,     // ++
  DECREMENT,     // --
  ARROW_L,       // <-
  ARROW_R,       // ->

  // Literals!
  IDENTIFIER,
  STRING,
  NUMBER,
  INAPPEND,

  // Keywords!
  AND, CLASS, ELSE,
  TK_FALSE, FN, FOR,
  IF, NIL, OR, INFO,
  TK_INPUT, RETURN, SUPER, 
  TK_THIS, TK_TRUE, HAVE,
  STATIC, LOOP, MODULE, 
  INCLUDE, SLEEP, EXIT, 
  CONTINUE, BREAK, BIT_AND,
  BIT_OR, MATCH, CASE, DEFAULT, ENUM, STRUCT,

  // Types!
  INT, FLOAT,
  STRING_TYPE,
  TK_BOOL,
  ANY,

  ERROR_TOKEN,
  // End of file!
  EOF_TOKEN
};

struct Token {
  std::string lexeme;
  const char* start;
  TokenKind kind;
  int column;
  size_t length;
  int line;
  Types value;
};

}

const char* get_source_line_start(int line);
void init_tokenizer(const char* source);
Element::Token scan_token();

