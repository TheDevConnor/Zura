#ifndef AZURA_PARSER_H
#define AZURA_PARSER_H

#include "object.h"
#include "vm.h"

ObjFunction* compile(const char* source);
void mark_compiler_roots();

#endif // AZURA_PARSER_H