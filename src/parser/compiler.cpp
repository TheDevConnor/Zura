#include <iostream>

#include "../helper/errors/parser_error.hpp"
#include "semantics/expr/expr.hpp"
#include "../common.hpp"
#include "compiler.hpp"

using namespace Zura;
using namespace Element;

bool ParserClass::compile(const char *source, Chunk *chunk) {
    init_tokenizer(source);
    ParserClass::compiling_chunk = chunk;

    ParserClass::advance();

    while (!ParserClass::match(EOF_TOKEN)) {
        ParserClass::declaration();
    }

    ParserClass::endCompiler();

    return !ParserClass::parser_error;
}
