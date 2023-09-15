#include "../semantics/expr/expr.hpp"
#include "precedence.hpp"

using namespace Zura;

void Prec::ParsePrecedence(Precedence precedence) {
    parserClass.advance();
    switch (parserClass.parser.previous.kind) {
        case LEFT_PAREN: Expr::grouping(); break;
        case MINUS: Expr::unary(); break;
        case NUMBER: Expr::number(); break;
        default: return;
    }
}

ParseRule* Prec::getRule(TokenKind kind) {
    return &rules[kind];
}

Prec::Prec() { 
    using namespace Zura::Expr;
    rules = {
        {LEFT_PAREN,   {grouping, nullptr, Precedence::NONE}}, // (
        {RIGHT_PAREN,   {nullptr, nullptr, Precedence::NONE}}, // )

        {LEFT_BRACE,    {nullptr, nullptr, Precedence::NONE}}, // {
        {RIGHT_BRACE,   {nullptr, nullptr, Precedence::NONE}}, // }

        {LEFT_BRACKET,  {nullptr, nullptr, Precedence::NONE}}, // [
        {RIGHT_BRACKET, {nullptr, nullptr, Precedence::NONE}}, // ]

        {COMMA,         {nullptr, nullptr, Precedence::NONE}}, // ,
        {DOT,           {nullptr, nullptr, Precedence::NONE}}, // .
        {MINUS,         {nullptr,  binary, Precedence::NONE}}, // -
        {PLUS,          {nullptr,  binary, Precedence::NONE}}, // +
        {SEMICOLON,     {nullptr, nullptr, Precedence::NONE}}, // ;
        {SLASH,         {nullptr,  binary, Precedence::NONE}}, // /
        {STAR,          {nullptr, nullptr, Precedence::NONE}}, // *
        {HASHTAG,       {nullptr, nullptr, Precedence::NONE}}, // #
        {COLON,         {nullptr, nullptr, Precedence::NONE}}, // :
        {MODULO,        {nullptr,  binary, Precedence::NONE}}, // %
        {DOLLAR,        {nullptr, nullptr, Precedence::NONE}}, // $
        {POWER,         {nullptr,  binary, Precedence::NONE}}, // **
        {TILDE,         {nullptr, nullptr, Precedence::NONE}}, // ~
        {AT,            {nullptr, nullptr, Precedence::NONE}}, // @

        {BANG,          {nullptr, nullptr, Precedence::NONE}}, // !
        {BANG_EQUAL,    {nullptr, nullptr, Precedence::NONE}}, // !=
        {EQUAL,         {nullptr, nullptr, Precedence::NONE}}, // =
        {GREATER,       {nullptr, nullptr, Precedence::NONE}}, // >
        {GREATER_EQUAL, {nullptr, nullptr, Precedence::NONE}}, // >=
        {LESS,          {nullptr, nullptr, Precedence::NONE}}, // <
        {LESS_EQUAL,    {nullptr, nullptr, Precedence::NONE}}, // <=
        {WALRUS,        {nullptr, nullptr, Precedence::NONE}}, // :=
        {EXTENDS,       {nullptr, nullptr, Precedence::NONE}}, // extends
        {INCREMENT,     {nullptr, nullptr, Precedence::NONE}}, // ++
        {DECREMENT,     {nullptr, nullptr, Precedence::NONE}}, // --
        {ARROW_L,       {nullptr, nullptr, Precedence::NONE}}, // <-
        {ARROW_R,       {nullptr, nullptr, Precedence::NONE}}, // ->

        {IDENTIFIER,    {nullptr, nullptr, Precedence::NONE}}, // identifier
        {STRING,        {nullptr, nullptr, Precedence::NONE}}, // string
        {NUMBER,        { number, nullptr, Precedence::NONE}}, // number
        {INAPPEND,      {nullptr, nullptr, Precedence::NONE}}, // inappend

        {CLASS,         {nullptr, nullptr, Precedence::NONE}}, // class
        {ELSE,          {nullptr, nullptr, Precedence::NONE}}, // else
        {TK_FALSE,      {nullptr, nullptr, Precedence::NONE}}, // false
        {FN,            {nullptr, nullptr, Precedence::NONE}}, // fn
        {FOR,           {nullptr, nullptr, Precedence::NONE}}, // for
        {IF,            {nullptr, nullptr, Precedence::NONE}}, // if
        {NIL,           {nullptr, nullptr, Precedence::NONE}}, // nil
        {INFO,          {nullptr, nullptr, Precedence::NONE}}, // info
        {TK_INPUT,      {nullptr, nullptr, Precedence::NONE}}, // input
        {RETURN,        {nullptr, nullptr, Precedence::NONE}}, // return
        {SUPER,         {nullptr, nullptr, Precedence::NONE}}, // super
        {TK_THIS,       {nullptr, nullptr, Precedence::NONE}}, // this
        {TK_TRUE,       {nullptr, nullptr, Precedence::NONE}}, // true
        {HAVE,          {nullptr, nullptr, Precedence::NONE}}, // have
        {STATIC,        {nullptr, nullptr, Precedence::NONE}}, // static
        {LOOP,          {nullptr, nullptr, Precedence::NONE}}, // loop
        {MODULE,        {nullptr, nullptr, Precedence::NONE}}, // module
        {INCLUDE,       {nullptr, nullptr, Precedence::NONE}}, // include
        {SLEEP,         {nullptr, nullptr, Precedence::NONE}}, // sleep
        {EXIT,          {nullptr, nullptr, Precedence::NONE}}, // exit
        {CONTINUE,      {nullptr, nullptr, Precedence::NONE}}, // continue
        {BREAK,         {nullptr, nullptr, Precedence::NONE}}, // break
        {BIT_AND,       {nullptr, nullptr, Precedence::NONE}}, // &
        {BIT_OR,        {nullptr, nullptr, Precedence::NONE}}, // |
        {MATCH,         {nullptr, nullptr, Precedence::NONE}}, // match
        {CASE,          {nullptr, nullptr, Precedence::NONE}}, // case
        {DEFAULT,       {nullptr, nullptr, Precedence::NONE}}, // default
        {ENUM,          {nullptr, nullptr, Precedence::NONE}}, // enum
        {STRUCT,        {nullptr, nullptr, Precedence::NONE}}, // struct

        {I8,            {nullptr, nullptr, Precedence::NONE}}, // i8
        {I16,           {nullptr, nullptr, Precedence::NONE}}, // i16
        {I32,           {nullptr, nullptr, Precedence::NONE}}, // i32
        {I64,           {nullptr, nullptr, Precedence::NONE}}, // i64
        {I128,          {nullptr, nullptr, Precedence::NONE}}, // i128

        {F32,           {nullptr, nullptr, Precedence::NONE}}, // f32
        {F64,           {nullptr, nullptr, Precedence::NONE}}, // f64
        {F128,          {nullptr, nullptr, Precedence::NONE}}, // f128

        {TK_BOOL,       {nullptr, nullptr, Precedence::NONE}}, // bool
        {ANY,           {nullptr, nullptr, Precedence::NONE}}, // any

        {ERROR_TOKEN,   {nullptr, nullptr, Precedence::NONE}}, // ERROR
        {EOF_TOKEN,     {nullptr, nullptr, Precedence::NONE}}, // EOF
    };
}
