#include <cstring>

#include "../common.hpp"
#include "../debug/debug.hpp"
#include "../helper/errors/parser_error.hpp"
#include "../lexer/tokens.hpp"
#include "../opCode/chunk.hpp"
#include "../types/type.hpp"
#include "compiler.hpp"

using namespace Zura;

uint8_t makeConstant(Value value)
{
    size_t constant = addConstants(parserClass.currentChunk(), value);
    if (constant > UINT8_MAX) {
        parserClass.errorAtCurrent("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

void ParserClass::errorAtCurrent(const std::string& message)
{
    ParserError::errorAt(&ParserClass::parser.current, message);
}

void ParserClass::advance()
{
    ParserClass::parser.previous = ParserClass::parser.current;

    while (true) {
        ParserClass::parser.current = scan_token();
        if (ParserClass::parser.current.kind != ERROR_TOKEN)
            break;

        ParserClass::errorAtCurrent(ParserClass::parser.current.start);
    }
}

void ParserClass::consume(TokenKind kind, const std::string& message)
{
    if (ParserClass::parser.current.kind == kind) {
        ParserClass::advance();
        return;
    }

    ParserClass::errorAtCurrent(message);
}

void ParserClass::emitByte(uint8_t byte)
{
    writeChunk(ParserClass::currentChunk(), byte, ParserClass::parser.previous.line);
}

void ParserClass::emitBytes(uint8_t byte1, uint8_t byte2)
{
    ParserClass::emitByte(byte1);
    ParserClass::emitByte(byte2);
}

void ParserClass::emitReturn()
{
    ParserClass::emitByte(OP_RETURN);
}

void ParserClass::emitConstant(Value value)
{
    ParserClass::emitBytes(OP_CONSTANT, makeConstant(value));
}

void ParserClass::endCompiler()
{
    ParserClass::emitReturn();
#ifdef DEBUG_PRINT_CODE
    disassembleChunk(ParserClass::currentChunk(), "code");
#endif
}

bool ParserClass::match(TokenKind kind)
{
    if (ParserClass::parser.current.kind != kind)
        return false;

    ParserClass::advance();
    return true;
}
