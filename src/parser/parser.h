#ifndef AZURA_PARSER_H
#define AZURA_PARSER_H

#include "object.h"
#include "vm.h"

bool compile(const char* source, Chunk* chunk);

#endif // AZURA_PARSER_H