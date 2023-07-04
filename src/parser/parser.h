#ifndef AZURA_PARSER_H
#define AZURA_PARSER_H

#include "object.h"
#include "vm.h"

ObjFunction* compile(const char* source);

#endif // AZURA_PARSER_H