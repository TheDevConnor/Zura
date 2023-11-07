#pragma once

#include <cstdint>
#include <string>

#include "precedence/precedence.hpp"
#include "../lexer/tokens.hpp"
#include "../types/type.hpp"
#include "../hash/table.hpp"
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
        HashTable::Table stringConstants;

        bool parser_error = false;

        Chunk* currentChunk() {
            return compiling_chunk;
        }

        void declaration();
        void statement();

        void emitByte(uint8_t byte);
        void emitBytes(uint8_t byte1, uint8_t byte2);
        void emitReturn();
        void emitConstant(Value value);
        void endCompiler();
        void errorAtCurrent(const std::string& message);
        void advance();
        void consume(TokenKind kind, const std::string& message);
        bool match(TokenKind kind);
        bool compile(const char* source, Chunk* chunk);
    };
}
uint8_t makeConstant(Value value);

inline Zura::ParserClass parserClass;
