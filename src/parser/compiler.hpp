#pragma once

#include <cstdint>
#include <string>

#include "precedence/precedence.hpp"
#include "../lexer/tokens.hpp"
#include "../types/type.hpp"
#include "../vm/vm.hpp"

namespace Zura {
    class ParserClass {
    public:
        struct Parser {
            Token current;
            Token previous;
        };

        Parser parser;
        Chunk* compiling_chunk;

        Chunk* currentChunk() {
            return compiling_chunk;
        }

        void emitByte(uint8_t byte);
        void emitBytes(uint8_t byte1, uint8_t byte2);
        void emitReturn();
        void emitConstant(Value value);
        void endCompiler();
        void errorAtCurrent(const std::string& message);
        void advance();
        void consume(TokenKind kind, const std::string& message);
        bool compile(const char* source, Chunk* chunk);
    };
};

inline Zura::ParserClass parserClass;

static uint8_t makeConstant(Value value);