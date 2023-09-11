#include <iostream>

#include "../helper/errors/parser_error.hpp"
#include "semantics/expr/expr.hpp"
#include "../common.hpp"
#include "compiler.hpp"

using namespace Zura;

bool ParserClass::compile(const char *source, Chunk *chunk) {
    init_tokenizer(source);
    ParserClass::compiling_chunk = chunk;

    ParserClass::advance();
    expr.expression();
    ParserClass::consume(EOF_TOKEN, "Expected end of expression");
    ParserClass::endCompiler();

    // There was no parser error.
    return true;
}