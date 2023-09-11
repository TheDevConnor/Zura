#pragma once

#include <unordered_map>

#include "../semantics/expr/expr.hpp"
#include "../../lexer/tokens.hpp"

namespace Zura {
    class Prec {
    public:
        enum Precedence {
            NONE,
            ASSIGNMENT,  // =
            OR,          // or
            AND,         // and
            EQUALITY,    // == !=
            COMPARISON,  // < > <= >=
            TERM,        // + -
            FACTOR,      // * /
            UNARY,       // ! -
            CALL,        // . ()
            PRIMARY
        };

        void ParsePrecedence(Precedence precedence);

        typedef void (*ParseFn)();

        struct ParseRule {
            ParseFn prefix;
            ParseFn infix;
            Precedence precedence;
        };

        ParseRule* getRule(TokenKind kind) {
            return &rules[kind];
        }

        std::unordered_map<TokenKind, ParseRule> rules = {
            {LEFT_PAREN,    ParseRule{nullptr, nullptr, Precedence::NONE}}, // (
            {RIGHT_PAREN,   ParseRule{nullptr, nullptr, Precedence::NONE}}, // )
            
            {LEFT_BRACE,    ParseRule{nullptr, nullptr, Precedence::NONE}}, // {
            {RIGHT_BRACE,   ParseRule{nullptr, nullptr, Precedence::NONE}}, // }
            
            {LEFT_BRACKET,  ParseRule{nullptr, nullptr, Precedence::NONE}}, // [
            {RIGHT_BRACKET, ParseRule{nullptr, nullptr, Precedence::NONE}}, // ]

            {COMMA,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // ,
            {DOT,           ParseRule{nullptr, nullptr, Precedence::NONE}}, // .
            {MINUS,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // -
            {PLUS,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // +
            {SEMICOLON,     ParseRule{nullptr, nullptr, Precedence::NONE}}, // ;
            {SLASH,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // /
            {STAR,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // *
            {HASHTAG,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // #
            {COLON,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // :
            {MODULO,        ParseRule{nullptr, nullptr, Precedence::NONE}}, // %
            {DOLLAR,        ParseRule{nullptr, nullptr, Precedence::NONE}}, // $
            {POWER,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // **
            {TILDE,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // ~
            {AT,            ParseRule{nullptr, nullptr, Precedence::NONE}}, // @

            {BANG,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // !
            {BANG_EQUAL,    ParseRule{nullptr, nullptr, Precedence::NONE}}, // !=
            {EQUAL,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // =
            {GREATER,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // >
            {GREATER_EQUAL, ParseRule{nullptr, nullptr, Precedence::NONE}}, // >=
            {LESS,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // <
            {LESS_EQUAL,    ParseRule{nullptr, nullptr, Precedence::NONE}}, // <=
            {WALRUS,        ParseRule{nullptr, nullptr, Precedence::NONE}}, // :=
            {EXTENDS,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // extends
            {INCREMENT,     ParseRule{nullptr, nullptr, Precedence::NONE}}, // ++
            {DECREMENT,     ParseRule{nullptr, nullptr, Precedence::NONE}}, // --
            {ARROW_L,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // <-
            {ARROW_R,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // ->

            {IDENTIFIER,    ParseRule{nullptr, nullptr, Precedence::NONE}}, // identifier
            {STRING,        ParseRule{nullptr, nullptr, Precedence::NONE}}, // string
            {NUMBER,        ParseRule{nullptr, nullptr, Precedence::NONE}}, // number
            {INAPPEND,      ParseRule{nullptr, nullptr, Precedence::NONE}}, // inappend

            {CLASS,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // class
            {ELSE,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // else
            {TK_FALSE,      ParseRule{nullptr, nullptr, Precedence::NONE}}, // false
            {FN,            ParseRule{nullptr, nullptr, Precedence::NONE}}, // fn
            {FOR,           ParseRule{nullptr, nullptr, Precedence::NONE}}, // for
            {IF,            ParseRule{nullptr, nullptr, Precedence::NONE}}, // if
            {NIL,           ParseRule{nullptr, nullptr, Precedence::NONE}}, // nil
            {INFO,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // info
            {TK_INPUT,      ParseRule{nullptr, nullptr, Precedence::NONE}}, // input
            {RETURN,        ParseRule{nullptr, nullptr, Precedence::NONE}}, // return
            {SUPER,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // super
            {TK_THIS,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // this
            {TK_TRUE,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // true
            {HAVE,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // have
            {STATIC,        ParseRule{nullptr, nullptr, Precedence::NONE}}, // static
            {LOOP,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // loop
            {MODULE,        ParseRule{nullptr, nullptr, Precedence::NONE}}, // module
            {INCLUDE,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // include
            {SLEEP,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // sleep
            {EXIT,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // exit
            {CONTINUE,      ParseRule{nullptr, nullptr, Precedence::NONE}}, // continue
            {BREAK,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // break
            {BIT_AND,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // &
            {BIT_OR,        ParseRule{nullptr, nullptr, Precedence::NONE}}, // |
            {MATCH,         ParseRule{nullptr, nullptr, Precedence::NONE}}, // match
            {CASE,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // case
            {DEFAULT,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // default
            {ENUM,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // enum
            {STRUCT,        ParseRule{nullptr, nullptr, Precedence::NONE}}, // struct

            {I8,            ParseRule{nullptr, nullptr, Precedence::NONE}}, // i8
            {I16,           ParseRule{nullptr, nullptr, Precedence::NONE}}, // i16
            {I32,           ParseRule{nullptr, nullptr, Precedence::NONE}}, // i32
            {I64,           ParseRule{nullptr, nullptr, Precedence::NONE}}, // i64
            {I128,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // i128
            
            {F32,           ParseRule{nullptr, nullptr, Precedence::NONE}}, // f32
            {F64,           ParseRule{nullptr, nullptr, Precedence::NONE}}, // f64
            {F128,          ParseRule{nullptr, nullptr, Precedence::NONE}}, // f128

            {TK_BOOL,       ParseRule{nullptr, nullptr, Precedence::NONE}}, // bool
            {ANY,           ParseRule{nullptr, nullptr, Precedence::NONE}}, // any

            {ERROR_TOKEN,   ParseRule{nullptr, nullptr, Precedence::NONE}}, // ERROR
            {EOF_TOKEN,     ParseRule{nullptr, nullptr, Precedence::NONE}}, // EOF
        };
    };
}

inline Zura::Prec prec;