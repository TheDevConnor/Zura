#ifndef AZURA_PARSER_H
#define AZURA_PARSER_H

#include "../compiler/object.h"
#include "../vm/vm.h"

ObjFunction* compile(const char* source);
void mark_compiler_roots();

#endif // AZURA_PARSER_H