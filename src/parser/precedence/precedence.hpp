#pragma once

#include <unordered_map>
#include <cstdint>

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
            FACTOR,      // * / % **
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

        uint8_t identifierConstant(Element::Token* name);
        uint8_t parseVariable(const char* errorMessage);
        void defineVariable(uint8_t global);

        ParseRule* getRule(Element::TokenKind kind);

        std::unordered_map<Element::TokenKind, ParseRule> rules;

        Prec();
    };
}

inline Zura::Prec prec;
