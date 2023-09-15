#pragma once

#include <unordered_map>

#include "../../lexer/tokens.hpp"

namespace Zura {
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

    typedef void (*ParseFn)();

    struct ParseRule {
        ParseFn prefix;
        ParseFn infix;
        Precedence precedence;
    };

    class Prec {
    public:
        void ParsePrecedence(Precedence precedence);

        ParseRule* getRule(TokenKind kind);

        std::unordered_map<TokenKind, ParseRule> rules;

        Prec();
    };
}

inline Zura::Prec prec;