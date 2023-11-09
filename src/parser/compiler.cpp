#include <iostream>

#include "../common.hpp"
#include "../hash/table.hpp"
#include "../helper/errors/parser_error.hpp"
#include "compiler.hpp"
#include "semantics/expr/expr.hpp"

using namespace Zura;

bool ParserClass::compile(const char* source, Chunk* chunk)
{
    init_tokenizer(source);

    ParserClass::compiling_chunk = chunk;
    parserClass.parser_error     = false;
    HashTable::initTable(&parserClass.stringConstants);

    ParserClass::advance();

    while (!ParserClass::match(EOF_TOKEN)) {
        ParserClass::declaration();
    }

    ParserClass::endCompiler();
    HashTable::freeTable(&parserClass.stringConstants);
    return !ParserClass::parser_error;
}