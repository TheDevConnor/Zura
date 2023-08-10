#pragma once

using namespace std;

typedef enum {
  // Single-character Tokens!
  LEFT_PAREN,
  RIGHT_PAREN, // ( )
  LEFT_BRACE,
  RIGHT_BRACE, // { }
  LEFT_BRACKET,
  RIGHT_BRACKET, // [ ]
  COMMA,
  DOT,
  MINUS,
  PLUS, // , . - +
  SEMICOLON,
  SLASH,
  STAR, // ; / *
  HASHTAG,
  COLON,
  MODULO, // # : %
  DOLLAR,
  POWER,
  TILDE, // $ ^

  // One or two character tokens!
  BANG,
  BANG_EQUAL, // ! !=
  EQUAL,
  EQUAL_EQUAL, // = ==
  GREATER,
  GREATER_EQUAL, // > >=
  LESS,
  LESS_EQUAL, // < <=
  WALRUS,
  INHERITANCE, // := extends
  INCREMENT,
  DECREMENT, // ++ --

  // Literals!
  IDENTIFIER,
  STRING,
  NUMBER,
  INAPPEND,

  // Keywords!
  AND,
  CLASS,
  ELSE,
  TK_FALSE,
  FUNC,
  FOR,
  IF,
  NIL,
  OR,
  INFO,
  TK_INPUT,
  RETURN,
  SUPER,
  TK_THIS,
  TK_TRUE,
  VAR,
  STATIC,
  WHILE,
  MODULE,
  INCLUDE,
  SLEEP,
  EXIT,
  CONTINUE,
  BREAK,
  BIT_AND,
  BIT_OR,
  SWITCH,
  CASE,
  DEFAULT,

  // Types!
  I8,
  I16,
  I32,
  I64,
  I128, // ints
  F32,
  F64,
  F128, // floats
  TK_BOOL,
  STRING_TYPE, // bool and string
  TK_VOID,     // void

  ERROR_TOKEN,
  // End of file!
  EOF_TOKEN
} TokenKind;

typedef struct {
  TokenKind kind;
  const char *start;
  int length;
  int line;
  int column;
} Token;

void init_tokenizer(const char *source);
const char *get_source_line_start(int line);
Token scan_token();
