#pragma once

#include <string>

typedef enum {
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
  I8, I16, I32, I64, I128,
  F32, F64, F128,
  TK_BOOL,
  ANY,

  ERROR_TOKEN,
  // End of file!
  EOF_TOKEN
} TokenKind;

typedef struct {
  TokenKind kind;
  const char* start;
  int length;
  int line;
  int column;
} Token;

const char* get_source_line_start(int line);
void init_tokenizer(const char* source);
Token scan_token();