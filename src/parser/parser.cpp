#include "lexer/tokens.h"

#include "helper/parser_error.h"
#include "helper/parser_struct.h"
#include "parser.h"

Parser parser;
Compiler* current = nullptr;
ClassCompiler* current_class = nullptr;
